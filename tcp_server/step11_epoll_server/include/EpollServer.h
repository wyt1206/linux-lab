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

    void initSocket();

    void initEpoll();


    void acceptConnection();


    void handleRead(int client_fd);


    void setNonBlocking(int fd);



private:

    int port_;

    int listen_fd_;

    int epoll_fd_;


    static constexpr int MAX_EVENTS = 10;


    struct epoll_event events_[MAX_EVENTS];

};


#endif