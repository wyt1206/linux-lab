#include "EventLoop.h"
#include "Channel.h"

#include <unistd.h>
#include <cstring>


EventLoop::EventLoop()
{

    epollfd_ = epoll_create1(0);

}


EventLoop::~EventLoop()
{

    close(epollfd_);

}



void EventLoop::updateChannel(Channel* channel)
{

    epoll_event event;

    memset(&event,0,sizeof(event));


    event.events =
        channel->events();


    /*
       注意这里

       不再保存 fd

       保存 Channel 指针

    */

    event.data.ptr = channel;



    epoll_ctl(
        epollfd_,
        EPOLL_CTL_ADD,
        channel->fd(),
        &event
    );

}



void EventLoop::loop()
{

    while(true)
    {

        int n =
            epoll_wait(
                epollfd_,
                events_,
                MAX_EVENTS,
                -1
            );


        for(int i=0;i<n;i++)
        {

            Channel* channel =
                static_cast<Channel*>(
                    events_[i].data.ptr
                );


            channel->setEvents(
                events_[i].events
            );


            channel->handleEvent();

        }

    }

}