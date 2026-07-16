#ifndef EPOLL_SERVER_H
#define EPOLL_SERVER_H


#include "EventLoop.h"


#include <unordered_map>

#include <memory>

#include <string>


class Channel;




class EpollServer
{

public:

    explicit EpollServer(
        int port
    );


    ~EpollServer();



    void start();



private:


    void initSocket();


    void acceptConnection();


    void handleRead(
        int fd
    );


    void handleWrite(
        int fd
    );



    void setNonBlocking(
        int fd
    );


private:

    int port_;

    int listen_fd_;


    EventLoop loop_;



    std::unique_ptr<Channel>
    listenChannel_;



    std::unordered_map<
        int,
        std::unique_ptr<Channel>
    >
    channels_;



    std::unordered_map<
        int,
        std::string
    >
    writeBuffers_;


};


#endif