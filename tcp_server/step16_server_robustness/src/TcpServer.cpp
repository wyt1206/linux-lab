#include "TcpServer.h"

#include "Acceptor.h"
#include "TcpConnection.h"
#include "ThreadPool.h"

#include <iostream>

TcpServer::TcpServer(EventLoop* loop, int port) : loop_(loop), port_(port)
{

    acceptor_ = std::make_unique<Acceptor>(loop_, port_);

    /*
        Acceptor accepts new fd

        notify TcpServer
    */
    acceptor_->setNewConnectionCallback([this](int fd) { newConnection(fd); });

    threadPool_ = std::make_unique<ThreadPool>(4);
}

TcpServer::~TcpServer()
{
}

void TcpServer::start()
{

    std::cout << "TcpServer started" << std::endl;
}

void TcpServer::newConnection(int fd)
{

    std::cout << "new connection fd=" << fd << std::endl;

    auto conn =
        std::make_shared<TcpConnection>(loop_, this, threadPool_, clientfd);

    connections_[clientfd] = conn;

    conn->connectEstablished();
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