#include "Channel.h"
#include "EventLoop.h"


#include <sys/epoll.h>



Channel::Channel(
    EventLoop* loop,
    int fd
)
:
loop_(loop),
fd_(fd),
events_(0),
revents_(0)
{

}



int Channel::fd() const
{
    return fd_;
}



uint32_t Channel::events() const
{
    return events_;
}



void Channel::setEvents(
    uint32_t events
)
{
    events_=events;
}



void Channel::setRevents(
    uint32_t rev
)
{
    revents_=rev;
}



void Channel::enableReading()
{

    events_ |= EPOLLIN;


    loop_->updateChannel(this);

}



void Channel::setReadCallback(
    Callback cb
)
{
    readCallback_=std::move(cb);
}




void Channel::handleEvent()
{

    if(revents_ & EPOLLIN)
    {

        if(readCallback_)
        {
            readCallback_();
        }

    }

}