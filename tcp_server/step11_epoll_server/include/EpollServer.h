#ifndef EPOLL_SERVER_H
#define EPOLL_SERVER_H

#include <sys/epoll.h>


class EpollServer
{

public:

    explicit EpollServer(int port);

    ~EpollServer();


    void start();


private:

    int port_;

    int server_fd_;

    int epoll_fd_;


    bool running_;


private:

    void setupSocket();

    void setupEpoll();

    void eventLoop();


    void acceptConnection();

};


#endif