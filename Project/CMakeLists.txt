cmake_minimum_required(VERSION 3.16)
project(ArbitrageEngine)

# 1. Bump to C++20 for std::jthread and modern atomics
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 2. Find Boost and OS-level Threads
find_package(Boost REQUIRED COMPONENTS system)
find_package(Threads REQUIRED)

#    Name the exe file | add the source code file name
add_executable(main_exe LatencyTest.cpp) 

# EXPLICITLY tell the compiler where the .hpp files are
target_include_directories(main_exe PRIVATE ${Boost_INCLUDE_DIRS})

# 3. Link them to your binary
target_link_libraries(main_exe 
    PRIVATE
    Boost::system
    Threads::Threads
)