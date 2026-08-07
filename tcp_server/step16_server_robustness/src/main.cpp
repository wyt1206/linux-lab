#include "EventLoop.h"
#include "TcpServer.h"

#include <iostream>

int main()
{

    EventLoop loop;

    TcpServer server(&loop, 8080);

    loop.queueInLoop([]() { std::cout << "task executed" << std::endl; });

    server.start();

    loop.loop();

    return 0;
}