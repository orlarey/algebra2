cmake_minimum_required(VERSION 3.15)
project(Algebra VERSION 1.0.0 LANGUAGES CXX)

# Set C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

# Header-only library
add_subdirectory(algebra)

# Main executable
add_executable(main main.cpp)
target_link_libraries(main algebra)

# Enable testing and add tests
enable_testing()
add_subdirectory(tests)

# Optional: Set output directory for binaries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)