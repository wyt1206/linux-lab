#include "EventLoop.h"
#include "Logger.h"
#include "Metrics.h"
#include "TcpServer.h"
#include <signal.h>

EventLoop* g_loop;

void handleSignal(int)
{
    if (g_loop)
    {
        g_loop->quit();
    }
}

int main()
{

    signal(SIGINT, handleSignal);

    EventLoop loop;

    g_loop = &loop;

    Logger::instance().enable(false);

    TcpServer server(&loop, 8080);

    Metrics::instance().setThreadPool(server.threadPool());

    server.start();

    loop.loop();

    Metrics::instance().print();

    return 0;
}
