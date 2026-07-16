#ifndef CONNECTION_H
#define CONNECTION_H


#include <string>
#include <memory>


class Channel;
class EventLoop;



class Connection
{

public:


    Connection(
        EventLoop* loop,
        int fd
    );


    ~Connection();



    int fd() const;



    void send(
        const std::string& data
    );


    void close();



private:


    void handleRead();


    void handleWrite();



private:


    EventLoop* loop_;


    int fd_;


    std::unique_ptr<Channel>
    channel_;



    std::string writeBuffer_;


};


#endif