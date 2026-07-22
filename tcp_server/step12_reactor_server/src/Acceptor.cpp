#include "Acceptor.h"

#include "Channel.h"
#include "EventLoop.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include <cstring>
#include <iostream>


Acceptor::~Acceptor() = default;



Acceptor::Acceptor(
    EventLoop* loop,
    int port
)
:
loop_(loop),
listenfd_(-1),
port_(port)
{

    /*
     * 1. 创建监听 socket
     */
    listenfd_ = socket(
        AF_INET,
        SOCK_STREAM,
        0
    );


    if(listenfd_ < 0)
    {
        std::cerr
            << "socket create failed"
            << std::endl;

        return;
    }



    /*
     * 2. 设置 SO_REUSEADDR
     *
     * 避免 server 重启时
     * bind: Address already in use
     */
    int opt = 1;

    setsockopt(
        listenfd_,
        SOL_SOCKET,
        SO_REUSEADDR,
        &opt,
        sizeof(opt)
    );



    /*
     * 3. bind
     */
    sockaddr_in addr{};

    addr.sin_family = AF_INET;

    addr.sin_addr.s_addr =
        INADDR_ANY;

    addr.sin_port =
        htons(port);



    if(bind(
        listenfd_,
        (sockaddr*)&addr,
        sizeof(addr)
    ) < 0)
    {
        std::cerr
            << "bind failed"
            << std::endl;

        close(listenfd_);
        return;
    }



    /*
     * 4. listen
     */
    if(listen(
        listenfd_,
        128
    ) < 0)
    {
        std::cerr
            << "listen failed"
            << std::endl;

        close(listenfd_);
        return;
    }



    std::cout
        << "Listening on port "
        << port_
        << std::endl;



    /*
     * 5. 创建 Channel
     *
     * listenfd 是一个 fd
     * Channel 负责监听它的事件
     */
    channel_ =
        std::make_unique<Channel>(
            loop_,
            listenfd_
        );



    /*
     * 6. 设置读事件回调
     *
     * epoll 返回 EPOLLIN
     * EventLoop 调 Channel
     * Channel 调这里
     */
    channel_->setReadCallback(
        [this]()
        {
            handleRead();
        }
    );



    /*
     * 7. 开启监听
     *
     * 注册 listenfd 到 epoll
     */
    channel_->enableReading();

}



void Acceptor::handleRead()
{

    /*
     * accept 新连接
     */
    int clientfd =
        accept(
            listenfd_,
            nullptr,
            nullptr
        );



    if(clientfd < 0)
    {
        std::cerr
            << "accept failed"
            << std::endl;

        return;
    }



    std::cout
        << "New client fd="
        << clientfd
        << std::endl;



    /*
     * 把 fd 交给 TcpServer
     */
    if(newConnectionCallback_)
    {
        newConnectionCallback_(
            clientfd
        );
    }

}



void Acceptor::setNewConnectionCallback(
    NewConnectionCallback cb
)
{
    newConnectionCallback_ =
        std::move(cb);
}