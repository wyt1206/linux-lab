#include "EventLoop.h"
#include "Metrics.h"
#include "TcpServer.h"
#include <signal.h>

EventLoop* g_loop;

void handleSignal(int)
{
    Metrics::instance().print();

    g_loop->quit();
}

int main()
{

    signal(SIGINT, handleSignal);

    EventLoop loop;

    g_loop = &loop;

    TcpServer server(&loop, 8080);

    server.start();

    loop.loop();

    Metrics::instance().print();

    return 0;
}