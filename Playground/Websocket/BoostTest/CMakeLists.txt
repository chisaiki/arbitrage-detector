cmake_minimum_required(VERSION 3.16)
project(MyWebSocketClient)

set(CMAKE_CXX_STANDARD 17)

# Tell CMake to find Boost on your system
find_package(Boost REQUIRED COMPONENTS system)

add_executable(client client.cpp)

# Link Boost::system into your binary
target_link_libraries(client 
    Boost::system
    pthread
)