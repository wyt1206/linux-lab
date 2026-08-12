#include "Acceptor.h"

#include "Channel.h"
#include "EventLoop.h"
#include "Logger.h"
#include "SocketUtil.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <cerrno>
#include <cstring>
#include <iostream>
#include <unistd.h>

Acceptor::Acceptor(EventLoop* loop, int port) : loop_(loop), listenfd_(-1)
{
    /*
        1. create listen socket
    */
    listenfd_ = socket(AF_INET, SOCK_STREAM, 0);

    if (listenfd_ < 0)
    {
        perror("socket");
        exit(1);
    }

    /*
        2. reuse address

        avoid:
        bind: Address already in use
    */
    int opt = 1;

    setsockopt(listenfd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    /*
        3. non-blocking listen socket
    */
    if (setNonBlocking(listenfd_) < 0)
    {
        perror("setNonBlocking");

        exit(1);
    }

    /*
        4. bind
    */
    sockaddr_in addr{};

    addr.sin_family = AF_INET;

    addr.sin_addr.s_addr = INADDR_ANY;

    addr.sin_port = htons(port);

    if (bind(listenfd_, (sockaddr*)&addr, sizeof(addr)) < 0)
    {
        perror("bind");

        exit(1);
    }

    /*
        5. listen
    */
    if (listen(listenfd_, 128) < 0)
    {
        perror("listen");

        exit(1);
    }

    Logger::instance().log("Acceptor listen on port " + std::to_string(port));

    /*
        6. create Channel

        listen fd is managed by Reactor
    */
    channel_ = std::make_unique<Channel>(loop_, listenfd_);

    channel_->setReadCallback([this]() { handleRead(); });

    channel_->enableReading();
}

Acceptor::~Acceptor()
{
    /*
        Channel destroyed first

        then close listen fd
    */
    if (listenfd_ >= 0)
    {
        close(listenfd_);
    }
}

void Acceptor::setNewConnectionCallback(NewConnectionCallback cb)
{
    callback_ = std::move(cb);
}

void Acceptor::handleRead()
{
    /*
        Because listenfd is non-blocking:

        accept() may return:

        -1
        errno = EAGAIN

        means:
        all pending connections are accepted

        NOT an error
    */
    while (true)
    {
        int clientfd = accept(listenfd_, nullptr, nullptr);

        if (clientfd < 0)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                return;
            }

            perror("accept");

            return;
        }

        /*
            client socket also needs
            non-blocking mode
        */
        if (setNonBlocking(clientfd) < 0)
        {
            perror("setNonBlocking client");

            close(clientfd);

            continue;
        }

        Logger::instance().log("accept client fd=" + std::to_string(clientfd));

        /*
            notify TcpServer

            TcpServer will create:

                TcpConnection
                    |
                    |
                 Channel
        */
        if (callback_)
        {
            callback_(clientfd);
        }
    }
}
