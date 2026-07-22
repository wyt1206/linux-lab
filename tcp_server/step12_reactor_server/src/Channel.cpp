#include "Channel.h"
#include "EventLoop.h"

#include <utility>


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
    revents_ = events;
}



void Channel::setReadCallback(
    EventCallback cb
)
{
    readCallback_ = std::move(cb);
}



void Channel::setWriteCallback(
    EventCallback cb
)
{
    writeCallback_ = std::move(cb);
}



void Channel::enableReading()
{
    events_ |= EPOLLIN;

    loop_->updateChannel(this);
}



void Channel::enableWriting()
{
    events_ |= EPOLLOUT;

    loop_->updateChannel(this);
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



    if(revents_ & EPOLLOUT)
    {
        if(writeCallback_)
        {
            writeCallback_();
        }
    }

}