#pragma once


class EventLoop;
class Channel;



class Acceptor {


public:

    Acceptor(
        EventLoop* loop,
        int port
    );


private:

    void handleRead();


    int listenfd_;


    Channel* channel_;

};