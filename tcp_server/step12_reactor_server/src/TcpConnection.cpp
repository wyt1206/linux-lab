#include "TcpConnection.h"

#include "Channel.h"
#include "EventLoop.h"


#include <sys/socket.h>
#include <unistd.h>

#include <iostream>



TcpConnection::TcpConnection(
    EventLoop* loop,
    int fd
)
:
loop_(loop),
fd_(fd)
{

    channel_ =
        std::make_unique<Channel>(
            loop_,
            fd_
        );


    channel_->setReadCallback(
        [this]()
        {
            handleRead();
        }
    );


    channel_->enableReading();

}




TcpConnection::~TcpConnection()
{

    close(fd_);

}




void TcpConnection::handleRead()
{

    char buf[1024];


    int n =
        recv(
            fd_,
            buf,
            sizeof(buf),
            0
        );



    if(n>0)
    {

        std::cout
            << "recv: "
            << std::string(buf,n)
            << std::endl;

    }


    else if(n==0)
    {


        std::cout
            << "client closed fd="
            << fd_
            << std::endl;



        loop_->removeChannel(
            channel_.get()
        );


    }

}