#include "TcpServer.h"

#include "EventLoop.h"
#include "Acceptor.h"
#include "TcpConnection.h"

#include <iostream>

TcpServer::~TcpServer() = default;

TcpServer::TcpServer(
    EventLoop* loop,
    int port
)
:
loop_(loop),
acceptor_(
    std::make_unique<Acceptor>(
        loop,
        port
    )
)
{

    acceptor_->setNewConnectionCallback(

        [this](int fd)
        {

            newConnection(fd);

        }

    );

}



void TcpServer::start()
{

    loop_->loop();

}



void TcpServer::newConnection(
    int fd
)
{

    std::cout
        << "new connection fd="
        << fd
        << std::endl;


    auto conn =
        std::make_unique<TcpConnection>(
            loop_,
            fd
        );


    connections_[fd]
        =
        std::move(conn);

}