#pragma once
#include "model/model_client.h"
#include <queue>
#include <string>

class ScriptedModelClient : public ModelClient {
public:
    explicit ScriptedModelClient(const std::string& script_path);
    
    using ModelClient::generate; // Prevent C++ Name Hiding compiler errors
    void generate(const Conversation& conv, TokenSink& sink) override;

private:
    std::queue<std::string> replies_;
};