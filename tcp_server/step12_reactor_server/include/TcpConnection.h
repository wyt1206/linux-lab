#pragma once


#include <memory>


class EventLoop;
class Channel;


class TcpConnection
{


public:

    TcpConnection(
        EventLoop* loop,
        int fd
    );


    ~TcpConnection();



    void handleRead();



private:


    EventLoop* loop_;

    int fd_;


    std::unique_ptr<Channel> channel_;

};