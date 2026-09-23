#include "model/scripted_client.h"
#include <fstream>
#include <stdexcept>

ScriptedModelClient::ScriptedModelClient(const std::string& script_path) {
    std::ifstream file(script_path);
    std::string line, current_reply;
    
    if (!file.is_open()) {
        replies_.push("Hello! This is a default fallback reply.<|end_conversation|>");
        return;
    }

    while (std::getline(file, line)) {
        if (line == "---") {
            if (!current_reply.empty()) {
                replies_.push(current_reply);
                current_reply.clear();
            }
        } else {
            if (line.find("role:") == std::string::npos && line.find("chunk:") == std::string::npos) {
                current_reply += line + "\n";
            }
        }
    }
    if (!current_reply.empty()) replies_.push(current_reply);
}

void ScriptedModelClient::generate(const Conversation& conv, TokenSink& sink) {
    if (replies_.empty()) {
        throw std::runtime_error("Exhausted all scripted blocks");
    }
    
    std::string reply = replies_.front();
    replies_.pop();
    
    sink.on_chunk(reply);
    sink.on_complete();
}