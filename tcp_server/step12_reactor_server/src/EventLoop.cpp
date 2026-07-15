#include "EventLoop.h"


#include <unistd.h>

#include <iostream>

#include <cstdlib>



EventLoop::EventLoop()
{

    epoll_fd_ =
        epoll_create1(0);


    if(epoll_fd_ < 0)
    {
        perror("epoll_create1");
        exit(1);
    }

}




EventLoop::~EventLoop()
{

    if(epoll_fd_ >= 0)
    {
        close(epoll_fd_);
    }

}






void EventLoop::addFd(
    int fd,
    uint32_t events
)
{

    epoll_event ev{};


    ev.events =
        events;


    ev.data.fd =
        fd;



    if(
        epoll_ctl(
            epoll_fd_,
            EPOLL_CTL_ADD,
            fd,
            &ev
        )
        < 0
    )
    {
        perror("epoll add");
    }

}







void EventLoop::modifyFd(
    int fd,
    uint32_t events
)
{

    epoll_event ev{};


    ev.events =
        events;


    ev.data.fd =
        fd;



    if(
        epoll_ctl(
            epoll_fd_,
            EPOLL_CTL_MOD,
            fd,
            &ev
        )
        < 0
    )
    {
        perror("epoll mod");
    }

}







void EventLoop::removeFd(
    int fd
)
{

    epoll_ctl(
        epoll_fd_,
        EPOLL_CTL_DEL,
        fd,
        nullptr
    );

}








int EventLoop::wait(
    epoll_event* events,
    int maxEvents
)
{

    return epoll_wait(
        epoll_fd_,
        events,
        maxEvents,
        -1
    );

}