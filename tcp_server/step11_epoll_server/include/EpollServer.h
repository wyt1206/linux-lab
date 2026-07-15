#ifndef EPOLL_SERVER_H
#define EPOLL_SERVER_H


#include <sys/epoll.h>

#include <unordered_map>

#include <string>



struct Connection
{
    int fd;

    std::string write_buffer;
};



class EpollServer
{

public:

    explicit EpollServer(int port);

    ~EpollServer();


    void start();



private:

    void initSocket();

    void initEpoll();


    void setNonBlocking(int fd);



    void acceptConnection();


    void handleRead(int fd);

    void handleWrite(int fd);



    void enableWrite(int fd);

    void disableWrite(int fd);



    void closeConnection(int fd);



private:

    int port_;

    int listen_fd_;

    int epoll_fd_;


    static constexpr int MAX_EVENTS = 10;


    struct epoll_event events_[MAX_EVENTS];



    std::unordered_map<int, Connection>
        connections_;

};


#endif