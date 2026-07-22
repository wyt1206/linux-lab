#pragma once

#include <memory>
#include <string>


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

    void send(
        const std::string& msg
    );


private:

    EventLoop* loop_;

    int fd_;

    std::unique_ptr<Channel> channel_;

};