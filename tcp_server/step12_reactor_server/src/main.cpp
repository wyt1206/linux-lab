#include "EventLoop.h"
#include "TcpServer.h"

int main()
{

    EventLoop loop;

    TcpServer server(&loop, 8080);

    loop.loop();

    return 0;
}