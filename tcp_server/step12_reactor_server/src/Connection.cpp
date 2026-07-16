#include "Connection.h"

#include "Channel.h"
#include "EventLoop.h"


#include <unistd.h>
#include <sys/socket.h>
#include <sys/epoll.h>


#include <iostream>
#include <cerrno>



Connection::Connection(
    EventLoop* loop,
    int fd
)
:
loop_(loop),
fd_(fd)
{


    channel_ =
        std::make_unique<Channel>(
            fd_
        );



    channel_->setReadCallback(
        [this]()
        {
            handleRead();
        }
    );


    channel_->setWriteCallback(
        [this]()
        {
            handleWrite();
        }
    );



    loop_->addChannel(
        channel_.get(),
        EPOLLIN | EPOLLET
    );

}




Connection::~Connection()
{

    close(fd_);

}






int Connection::fd() const
{
    return fd_;
}








void Connection::handleRead()
{

    char buffer[1024];


    while(true)
    {

        int n =
            recv(
                fd_,
                buffer,
                sizeof(buffer),
                0
            );


        if(n > 0)
        {

            send(
                std::string(
                    buffer,
                    n
                )
            );

        }


        else if(n == 0)
        {

            close();

            break;

        }


        else
        {

            if(errno == EAGAIN ||
               errno == EWOULDBLOCK)
            {
                break;
            }


            close();

            break;

        }

    }

}








void Connection::send(
    const std::string& data
)
{

    writeBuffer_
        += data;



    loop_->modifyChannel(
        channel_.get(),
        EPOLLIN |
        EPOLLOUT |
        EPOLLET
    );

}









void Connection::handleWrite()
{

    while(!writeBuffer_.empty())
    {

        int n =
            ::send(
                fd_,
                writeBuffer_.data(),
                writeBuffer_.size(),
                0
            );


        if(n > 0)
        {

            writeBuffer_.erase(
                0,
                n
            );

        }

        else
        {

            if(errno==EAGAIN)
                break;


            close();

            return;

        }

    }



    if(writeBuffer_.empty())
    {

        loop_->modifyChannel(
            channel_.get(),
            EPOLLIN |
            EPOLLET
        );

    }

}







void Connection::close()
{

    loop_->removeChannel(
        channel_.get()
    );


    ::close(fd_);

}
