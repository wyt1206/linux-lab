cmake_minimum_required(VERSION 3.10)


project(epoll_server)


set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)


include_directories(include)


add_executable(
    server

    src/main.cpp

    src/EpollServer.cpp
)