#include "Channel.h"

#include <sys/epoll.h>



Channel::Channel(
    int fd
)
:
fd_(fd)
{

}



int Channel::fd() const
{
    return fd_;
}




void Channel::setReadCallback(
    Callback cb
)
{
    readCallback_ = std::move(cb);
}





void Channel::setWriteCallback(
    Callback cb
)
{
    writeCallback_ = std::move(cb);
}






void Channel::handleEvent(
    uint32_t events
)
{

    if(
        events & EPOLLIN
    )
    {

        if(readCallback_)
        {
            readCallback_();
        }

    }



    if(
        events & EPOLLOUT
    )
    {

        if(writeCallback_)
        {
            writeCallback_();
        }

    }

}