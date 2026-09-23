#pragma once
#include "core/conversation.h"
#include "model/model_client.h"
#include <memory>
#include <iostream>

struct HarnessConfig {
    int max_turns = 20;
    std::string transcript_path = "";
};

class Harness {
public:
    Harness(std::unique_ptr<ModelClient> model, HarnessConfig cfg);
    StopReason run(std::istream& in, std::ostream& out);

private:
    std::unique_ptr<ModelClient> model_;
    Conversation                 conv_;
    HarnessConfig                cfg_;
};