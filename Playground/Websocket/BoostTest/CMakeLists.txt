# cmake_minimum_required(VERSION 3.16)
# project(MyWebSocketClient)

# set(CMAKE_CXX_STANDARD 17)

# # Tell CMake to find Boost on your system
# find_package(Boost REQUIRED COMPONENTS system)

# add_executable(client client.cpp)

# # Link Boost::system into your binary
# target_link_libraries(client 
#     Boost::system
#     pthread
# )

cmake_minimum_required(VERSION 3.16)
project(ArbitrageEngine)

# 1. Bump to C++20 for std::jthread and modern atomics
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 2. Find Boost and OS-level Threads
find_package(Boost REQUIRED COMPONENTS system)
find_package(Threads REQUIRED)

add_executable(client client.cpp)

# EXPLICITLY tell the compiler where the .hpp files are
target_include_directories(client PRIVATE ${Boost_INCLUDE_DIRS})

# 3. Link them to your binary
target_link_libraries(client 
    PRIVATE
    Boost::system
    Threads::Threads
)

