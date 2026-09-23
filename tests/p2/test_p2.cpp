#include "core/conversation.h"
#include "core/message.h"
#include <iostream>
#include <cassert>

#include "core/sentinel_scanner.h"

int main() {
    std::cout << "Testing Conversation Memory Limits...\n";

    // Test 1: Allocation and Growth (Forces capacity to double)
    Conversation conv1;
    for (int i = 0; i < 10; ++i) {
        conv1.append(Message(Role::User, "Test message " + std::to_string(i)));
    }
    assert(conv1.size() == 10);

    // Test 2: Copy Constructor (Must allocate new memory)
    Conversation conv2 = conv1;
    assert(conv2.size() == 10);
    assert(conv1.begin() != conv2.begin()); // Pointers MUST be different

    // Test 3: Move Constructor (Must steal pointer and zero source)
    Conversation conv3 = std::move(conv1);
    assert(conv3.size() == 10);
    assert(conv1.size() == 0);
    assert(conv1.begin() == nullptr); 

    std::cout << "All memory tests passed cleanly!\n";


    std::cout << "Testing SentinelScanner Boundaries...\n";
    
    std::string sentinel = "<|end_conversation|>";
    std::string safe_text = "This is a normal message.";
    
    // Loop through every possible split point of the sentinel
    for (size_t i = 1; i < sentinel.size(); ++i) {
        SentinelScanner scanner(sentinel);
        
        std::string part1 = sentinel.substr(0, i);
        std::string part2 = sentinel.substr(i);
        
        // Feed the safe text plus the first fragment of the sentinel
        auto out1 = scanner.feed(safe_text + part1);
        assert(out1.sentinel_found == false); // Sentinel isn't complete yet
        
        // Feed the remaining fragment of the sentinel
        auto out2 = scanner.feed(part2);
        assert(out2.sentinel_found == true); // Now it should trigger
        
        // Ensure the original text didn't get swallowed or corrupted
        assert(out1.safe_text + out2.safe_text == safe_text);
    }
    
    std::cout << "Scanner split boundary test passed cleanly!\n";

    return 0;
}