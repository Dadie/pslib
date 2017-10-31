cmake_minimum_required(VERSION 3.8)

project (pslib VERSION 0.1.1)

# Set compiler options
set (CMAKE_C_STANDARD 11)
set (CMAKE_C_COMPILER             "/usr/bin/clang")
set (CMAKE_C_FLAGS                "-Weverything -pthread")
set (CMAKE_C_FLAGS_DEBUG          "-g -fsanitize=undefined -fsanitize=address -fsanitize=leak")
set (CMAKE_C_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
set (CMAKE_C_FLAGS_RELEASE        "-O4 -DNDEBUG")
set (CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -g")

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_COMPILER             "/usr/bin/clang++")
set (CMAKE_CXX_FLAGS                "-Weverything -Wno-unused-parameter -Wno-weak-vtables -Wno-c++98-compat -Wno-padded -Wno-covered-switch-default")
set (CMAKE_CXX_FLAGS_DEBUG          "-g -fsanitize=undefined -fsanitize=address -fsanitize=leak")
set (CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
set (CMAKE_CXX_FLAGS_RELEASE        "-O4 -DNDEBUG")
set (CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")

set (CMAKE_SHARED_LINKER_FLAGS "-v -lboost_system")

find_package(Boost 1.65.0 REQUIRED COMPONENTS system filesystem)

enable_testing()
add_subdirectory (src)
add_subdirectory (test)
