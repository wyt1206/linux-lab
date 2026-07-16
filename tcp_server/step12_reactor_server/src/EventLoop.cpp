#include "EventLoop.h"

#include "Channel.h"


#include <unistd.h>

#include <cstdlib>

#include <iostream>




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

    close(epoll_fd_);

}








void EventLoop::addChannel(
    Channel* channel,
    uint32_t events
)
{

    epoll_event ev{};


    ev.events =
        events;


    ev.data.ptr =
        channel;



    epoll_ctl(
        epoll_fd_,
        EPOLL_CTL_ADD,
        channel->fd(),
        &ev
    );

}








void EventLoop::modifyChannel(
    Channel* channel,
    uint32_t events
)
{

    epoll_event ev{};


    ev.events =
        events;


    ev.data.ptr =
        channel;



    epoll_ctl(
        epoll_fd_,
        EPOLL_CTL_MOD,
        channel->fd(),
        &ev
    );

}








void EventLoop::removeChannel(
    Channel* channel
)
{

    epoll_ctl(
        epoll_fd_,
        EPOLL_CTL_DEL,
        channel->fd(),
        nullptr
    );

}








void EventLoop::loop()
{

    while(true)
    {

        int n =
            epoll_wait(
                epoll_fd_,
                events_,
                MAX_EVENTS,
                -1
            );



        for(int i=0;i<n;i++)
        {

            Channel* channel =
                static_cast<Channel*>(
                    events_[i]
                    .data
                    .ptr
                );


            channel->handleEvent(
                events_[i].events
            );

        }

    }

}