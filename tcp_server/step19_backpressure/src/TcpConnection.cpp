#include "TcpConnection.h"

#include "Channel.h"
#include "EventLoop.h"
#include "Logger.h"
#include "Metrics.h"
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

    // Step 18.6.3:
    // reserve write buffer to reduce reallocations
    writeBuffer_.reserve(8192);
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
            Metrics::instance().incrementRequestBuffers();

            std::string message(buffer, n);

            Logger::instance().log("received: " + message);

            auto self = shared_from_this();

            bool accepted = threadPool_->submit(
                [self, message = std::move(message)]()
                {
                    /*
                        worker thread
                    */
                    Logger::instance().log("processing: " + message);

                    /*
                        Step 18.3:
                        simulate CPU contention

                        emulate expensive computation
                    */
                    volatile uint64_t result = 0;

                    for (int i = 0; i < 50000000; i++)
                    {
                        result += i;
                    }

                    Metrics::instance().incrementResponseBuffers();

                    std::string response = "processed: " + message;

                    /*
                        switch back
                        to EventLoop thread
                    */
                    self->send(std::move(response));
                });
            if (!accepted)

            {

                Logger::instance().log(

                    "ThreadPool queue full, rejecting request");
            }
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

void TcpConnection::send(std::string msg)
{

    auto self = shared_from_this();

    loop_->runInLoop(
        [self, msg = std::move(msg)]()
        {
            if (self->state_ != ConnectionState::CONNECTED)
            {
                return;
            }

            Logger::instance().log("async send: " + msg);

            Metrics::instance().incrementWriteBufferAppends();

            self->writeBuffer_ += msg;

            Logger::instance().log("write buffer: " + self->writeBuffer_);

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
            Metrics::instance().incrementResponses();

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

    Logger::instance().log("connection closed fd=" + std::to_string(fd_));

    loop_->removeChannel(channel_.get());

    Metrics::instance().decrementConnections();

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
