#include "TcpServer.h"

#include "Acceptor.h"
#include "EventLoop.h"
#include "TcpConnection.h"

#include <iostream>

TcpServer::TcpServer(EventLoop* loop, int port) : loop_(loop)
{

    acceptor_ = std::make_unique<Acceptor>(loop_, port);

    acceptor_->setNewConnectionCallback([this](int fd) { newConnection(fd); });
}

TcpServer::~TcpServer()
{
}

void TcpServer::start()
{

    // Acceptor is already listening
    // after construction
}

void TcpServer::newConnection(int fd)
{

    std::cout << "new connection fd=" << fd << std::endl;

    auto conn = std::make_shared<TcpConnection>(loop_, this, fd);

    connections_[fd] = conn;
}

void TcpServer::removeConnection(int fd)
{
    auto it = connections_.find(fd);

    if (it != connections_.end())
    {

        connections_.erase(it);

        std::cout << "remove connection fd=" << fd << std::endl;
    }
}