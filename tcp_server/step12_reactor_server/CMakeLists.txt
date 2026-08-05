cmake_minimum_required(VERSION 3.10)


project(reactor-server)


set(CMAKE_CXX_STANDARD 17)



include_directories(include)



add_executable(server

    src/main.cpp

    src/EventLoop.cpp
    src/Channel.cpp

    src/Acceptor.cpp

    src/TcpConnection.cpp
    src/TcpServer.cpp
    src/SocketUtil.cpp
)