#include "TcpServer.h"

#include "Acceptor.h"
#include "Logger.h"
#include "Metrics.h"
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

    Logger::instance().log("TcpServer started");
}

ThreadPool* TcpServer::threadPool()
{
    return threadPool_.get();
}

void TcpServer::newConnection(int fd)
{

    Logger::instance().log("new connection fd=" + std::to_string(fd));

    Metrics::instance().incrementConnections();

    auto conn =
        std::make_shared<TcpConnection>(loop_, this, threadPool_.get(), fd);

    /*
        IMPORTANT:

        store shared_ptr first
        then call connectEstablished

    */

    connections_[fd] = conn;

    conn->connectEstablished();
}

void TcpServer::removeConnection(int fd)
{

    auto it = connections_.find(fd);

    if (it != connections_.end())
    {

        connections_.erase(it);

        Logger::instance().log("remove connection fd=" + std::to_string(fd));
    }
}

size_t TcpServer::connectionCount() const
{
    return connections_.size();
}
