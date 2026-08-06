#include "EventLoop.h"
#include "TcpServer.h"

int main()
{

    EventLoop loop;

    TcpServer server(&loop, 8080);

    server.start();

    loop.loop();

    return 0;
}