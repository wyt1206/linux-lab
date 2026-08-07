#include "TcpConnection.h"

#include "Channel.h"
#include "EventLoop.h"
#include "TcpServer.h"
#include "ThreadPool.h"

#include <sys/socket.h>
#include <unistd.h>

#include <errno.h>
#include <iostream>

TcpConnection::TcpConnection(EventLoop* loop, TcpServer* server,
                             ThreadPool* pool, int fd)
    : loop_(loop), server_(server), threadPool_(pool), fd_(fd),
      state_(ConnectionState::CONNECTING)
{

    channel_ = std::make_unique<Channel>(loop_, fd_);
}

TcpConnection::~TcpConnection()
{
    if (fd_ >= 0)
    {
        close(fd_);
    }
}

int TcpConnection::fd() const
{
    return fd_;
}

void TcpConnection::connectEstablished()
{

    state_ = ConnectionState::CONNECTED;

    std::weak_ptr<TcpConnection> weakSelf = shared_from_this();

    channel_->setReadCallback(
        [weakSelf]()
        {
            if (auto self = weakSelf.lock())
            {
                self->handleRead();
            }
        });

    channel_->setWriteCallback(
        [weakSelf]()
        {
            if (auto self = weakSelf.lock())
            {
                self->handleWrite();
            }
        });

    channel_->setCloseCallback(
        [weakSelf]()
        {
            if (auto self = weakSelf.lock())
            {
                self->handleClose();
            }
        });

    /*
        Register fd into epoll
    */
    channel_->enableReading();
}

void TcpConnection::handleRead()
{

    char buffer[4096];

    while (true)
    {

        int n = recv(fd_, buffer, sizeof(buffer), 0);

        if (n > 0)
        {

            std::string message(buffer, n);

            std::cout << "received: " << message << std::endl;

            auto self = shared_from_this();

            threadPool_->submit(
                [self, message]()
                {
                    /*
                        worker thread
                    */

                    std::cout << "processing: " << message << std::endl;

                    std::string response = "processed: " + message;

                    /*
                        switch back
                        to EventLoop thread
                    */

                    self->send(response);
                });
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

    auto self = shared_from_this();

    loop_->runInLoop(
        [self, msg]()
        {
            if (self->state_ != ConnectionState::CONNECTED)
            {
                return;
            }

            std::cout << "async send: " << msg << std::endl;

            self->writeBuffer_ += msg;

            std::cout << "write buffer: " << self->writeBuffer_ << std::endl;

            self->channel_->enableWriting();
        });
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

            if (errno == EAGAIN || errno == EWOULDBLOCK)
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

    if (state_ == ConnectionState::DISCONNECTED)
    {
        return;
    }

    state_ = ConnectionState::DISCONNECTING;

    std::cout << "connection closed fd=" << fd_ << std::endl;

    loop_->removeChannel(channel_.get());

    /*
        remove from TcpServer map

        shared_ptr count decreases
    */

    server_->removeConnection(fd_);

    state_ = ConnectionState::DISCONNECTED;
}

bool TcpConnection::connected() const
{
    return state_ == ConnectionState::CONNECTED;
}