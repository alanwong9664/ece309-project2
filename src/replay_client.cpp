#include "model/replay_client.h"
#include <fstream>
#include <stdexcept>

ReplayModelClient::ReplayModelClient(const std::string& transcript_path) {
    std::ifstream file(transcript_path);
    std::string line, current_content;
    bool is_assistant = false;

    while (std::getline(file, line)) {
        if (line == "---") {
            if (is_assistant && !current_content.empty()) {
                assistant_replies_.push_back(current_content);
            }
            current_content.clear();
            is_assistant = false;
        } else if (line.find("role:assistant") != std::string::npos) {
            is_assistant = true;
        } else if (line.find("role:user") != std::string::npos) {
            is_assistant = false;
        } else if (is_assistant) {
            current_content += line + "\n";
        }
    }
    // Catch a final block if the file doesn't end in ---
    if (is_assistant && !current_content.empty()) {
        assistant_replies_.push_back(current_content);
    }
}

void ReplayModelClient::generate(const Conversation& conv, TokenSink& sink) {
    if (current_turn_ >= assistant_replies_.size()) {
        throw std::runtime_error("Exhausted all replay blocks");
    }
    
    sink.on_chunk(assistant_replies_[current_turn_++]);
    sink.on_complete();
}