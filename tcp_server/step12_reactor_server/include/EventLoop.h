#ifndef EVENT_LOOP_H
#define EVENT_LOOP_H


#include <sys/epoll.h>


class EventLoop
{

public:

    EventLoop();

    ~EventLoop();


    void addFd(
        int fd,
        uint32_t events
    );


    void modifyFd(
        int fd,
        uint32_t events
    );


    void removeFd(
        int fd
    );


    int wait(
        epoll_event* events,
        int maxEvents
    );



private:

    int epoll_fd_;

};


#endif