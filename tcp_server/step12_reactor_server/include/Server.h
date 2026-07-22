#pragma once


class EventLoop;
class Acceptor;



class Server {


public:

    Server(int port);


    void start();



private:

    EventLoop* loop_;

    Acceptor* acceptor_;

};