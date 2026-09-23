cmake_minimum_required(VERSION 3.10)
project(MiniHarness CXX)

# C++17 is required for std::string_view used in the spec interfaces
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Mandatory AddressSanitizer and helpful debugging flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -g -Wall -Wextra")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")

# Tell the compiler where to find your header files
include_directories(include)

# Group the core source files (everything except main.cpp and tests)
set(CORE_SOURCES
    src/message.cpp
    src/conversation.cpp
    src/sentinel_scanner.cpp
    src/model_client.cpp
    src/scripted_client.cpp
    src/replay_client.cpp
    src/harness.cpp
)

# Target 1: The main executable (miniharness)
add_executable(miniharness src/main.cpp ${CORE_SOURCES})

# Target 2: The test suite required by the rubric
add_executable(test_p2 tests/p2/test_p2.cpp ${CORE_SOURCES})