cmake_minimum_required(VERSION 3.10)

project(step12_reactor_server)


set(CMAKE_CXX_STANDARD 17)


include_directories(include)



add_executable(
    server

    src/main.cpp

    src/EventLoop.cpp

    src/Channel.cpp

    src/EpollServer.cpp
)