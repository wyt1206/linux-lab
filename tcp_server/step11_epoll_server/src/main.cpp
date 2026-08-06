#include "EpollServer.h"

int main()
{

    EpollServer server(8080);

    server.start();

    return 0;
}