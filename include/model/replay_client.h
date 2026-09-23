#pragma once
#include "model/model_client.h"
#include <string>
#include <vector>

class ReplayModelClient : public ModelClient {
public:
    explicit ReplayModelClient(const std::string& transcript_path);
    
    using ModelClient::generate; 
    void generate(const Conversation& conv, TokenSink& sink) override;

private:
    std::vector<std::string> assistant_replies_;
    size_t current_turn_ = 0;
};