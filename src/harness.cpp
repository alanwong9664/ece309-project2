#include "harness/harness.h"
#include <fstream>
#include <string>
#include <stdexcept>

// A simple sink that prints to the terminal and records the full string
class ConsoleSink : public TokenSink {
public:
    ConsoleSink(std::ostream& out) : out_(out) {}
    
    void on_chunk(std::string_view chunk) override {
        out_ << chunk;
        out_.flush();
        full_reply_ += chunk;
    }
    
    void on_complete() override {
        out_ << "\n";
    }
    
    std::string get_reply() const { return full_reply_; }

private:
    std::ostream& out_;
    std::string full_reply_;
};

Harness::Harness(std::unique_ptr<ModelClient> model, HarnessConfig cfg)
    : model_(std::move(model)), cfg_(std::move(cfg)) {}

StopReason Harness::run(std::istream& in, std::ostream& out) {
    int turns = 0;
    std::string user_input;
    StopReason reason{StopReason::Kind::TurnLimit, "Turn limit reached"};

    while (turns < cfg_.max_turns) {
        out << "you> ";
        out.flush();

        // 1. Check for EOF (Ctrl-D)
        if (!std::getline(in, user_input)) {
            reason = {StopReason::Kind::UserExit, "User exited via EOF"};
            break; // Exit loop, but proceed to transcript saving
        }

        // 2. Append User Message
        conv_.append(Message(Role::User, user_input));

        // 3. Generate Assistant Reply
        out << "assistant> ";
        ConsoleSink sink(out);
        
        try {
            model_->generate(conv_, sink);
        } catch (const std::exception& e) {
            reason = {StopReason::Kind::ClientError, e.what()};
            break; // Exit loop, but proceed to transcript saving
        }

        // 4. Save Assistant Message
        conv_.append(Message(Role::Assistant, sink.get_reply()));

        turns++;
    }

    // 5. Save transcript if a path was provided in the config
    if (!cfg_.transcript_path.empty()) {
        std::ofstream out_file(cfg_.transcript_path);
        if (out_file.is_open()) {
            for (size_t i = 0; i < conv_.size(); ++i) {
                const Message& msg = conv_.at(i);
                out_file << "role:" << (msg.role() == Role::User ? "user" : "assistant") << "\n";
                out_file << msg.content() << "\n---\n";
            }
        } else {
            out << "[warning: failed to open transcript file for writing]\n";
        }
    }

    return reason;
}