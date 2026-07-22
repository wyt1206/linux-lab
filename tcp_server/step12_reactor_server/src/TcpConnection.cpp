#include "TcpConnection.h"

#include "Channel.h"
#include "EventLoop.h"


#include <unistd.h>

#include <iostream>


TcpConnection::TcpConnection(
    EventLoop* loop,
    int fd
)
:
loop_(loop),
fd_(fd),
channel_(
    std::make_unique<Channel>(
        loop,
        fd
    )
)
{

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

    char buffer[1024];


    int n =
        read(
            fd_,
            buffer,
            sizeof(buffer)
        );


    if(n > 0)
    {

        std::string msg(
            buffer,
            n
        );


        std::cout
            << "Received: "
            << msg
            << std::endl;


        send(msg);

    }
    else
    {

        std::cout
            << "Client disconnected"
            << std::endl;

    }

}



void TcpConnection::send(
    const std::string& msg
)
{

    write(
        fd_,
        msg.data(),
        msg.size()
    );

}