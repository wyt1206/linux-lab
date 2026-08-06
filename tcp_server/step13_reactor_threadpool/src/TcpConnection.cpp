#include "TcpConnection.h"

#include "Channel.h"
#include "EventLoop.h"
#include "TcpServer.h"

#include <sys/socket.h>
#include <unistd.h>

#include <sys/epoll.h>

#include <cerrno>
#include <iostream>

TcpConnection::TcpConnection(EventLoop* loop, TcpServer* server, int fd)
    : loop_(loop), server_(server), fd_(fd)
{

    channel_ = std::make_unique<Channel>(loop_, fd_);

    channel_->setReadCallback([this]() { handleRead(); });

    channel_->setWriteCallback([this]() { handleWrite(); });

    channel_->enableReading();
}

TcpConnection::~TcpConnection()
{
    close(fd_);
}

int TcpConnection::fd() const
{
    return fd_;
}

void TcpConnection::handleRead()
{
    char buffer[4096];

    while (true)
    {

        int n = recv(fd_, buffer, sizeof(buffer), 0);

        if (n > 0)
        {
            send(std::string(buffer, n));
        }
        else if (n == 0)
        {

            handleClose();

            return;
        }
        else
        {

            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                break;
            }

            handleClose();

            return;
        }
    }
}

void TcpConnection::send(const std::string& msg)
{
    writeBuffer_ += msg;

    channel_->enableWriting();
}

void TcpConnection::handleWrite()
{
    while (!writeBuffer_.empty())
    {

        int n = ::send(fd_, writeBuffer_.data(), writeBuffer_.size(), 0);

        if (n > 0)
        {

            writeBuffer_.erase(0, n);
        }
        else
        {

            if (errno == EAGAIN)
            {
                return;
            }

            handleClose();

            return;
        }
    }

    channel_->disableWriting();
}

void TcpConnection::handleClose()
{
    loop_->removeChannel(channel_.get());

    server_->removeConnection(fd_);
}