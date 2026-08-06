#include "EventLoop.h"
#include "Channel.h"


#include <unistd.h>
#include <iostream>


EventLoop::EventLoop()
{
    epollfd_ = epoll_create1(0);

    quit_ = false;
}


EventLoop::~EventLoop()
{
    close(epollfd_);
}



void EventLoop::updateChannel(Channel* channel)
{

    epoll_event ev{};

    ev.events =
        channel->events();


    ev.data.ptr = channel;


    epoll_ctl(
        epollfd_,
        EPOLL_CTL_ADD,
        channel->fd(),
        &ev
    );

}



void EventLoop::removeChannel(Channel* channel)
{

    epoll_ctl(
        epollfd_,
        EPOLL_CTL_DEL,
        channel->fd(),
        nullptr
    );

}



void EventLoop::loop()
{

    epoll_event events[10];


    while(!quit_)
    {

        int n =
            epoll_wait(
                epollfd_,
                events,
                10,
                -1
            );


        for(int i=0;i<n;i++)
        {

            Channel* ch =
                static_cast<Channel*>(
                    events[i].data.ptr
                );


            ch->setRevents(
                events[i].events
            );


            ch->handleEvent();

        }

    }

}