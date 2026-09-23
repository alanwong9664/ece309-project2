#include "harness/harness.h"
#include "model/scripted_client.h"
#include <iostream>
#include <memory>
#include <string>
#include <vector>

int main(int argc, char* argv[]) {
    HarnessConfig cfg;
    std::string script_path = "";

    // Parse command line arguments
    std::vector<std::string> args(argv + 1, argv + argc);
    for (size_t i = 0; i < args.size(); ++i) {
        if (args[i] == "--max-turns" && i + 1 < args.size()) {
            cfg.max_turns = std::stoi(args[++i]);
        } else if (args[i] == "--save" && i + 1 < args.size()) {
            cfg.transcript_path = args[++i];
        } else if (args[i] == "--script" && i + 1 < args.size()) {
            script_path = args[++i];
        }
    }

    // Initialize model and harness
    auto model = std::make_unique<ScriptedModelClient>(script_path);
    Harness harness(std::move(model), cfg);
    
    StopReason reason = harness.run(std::cin, std::cout);

    std::cout << "\n[conversation ended: " << reason.detail << "]\n";
    return 0;
}