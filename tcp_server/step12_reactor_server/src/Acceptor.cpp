#include "Acceptor.h"

#include "Channel.h"
#include "EventLoop.h"


#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>


Acceptor::Acceptor(
    EventLoop* loop,
    int port
)
{

    listenfd_=socket(
        AF_INET,
        SOCK_STREAM,
        0
    );


    sockaddr_in addr{};


    addr.sin_family =
        AF_INET;

    addr.sin_port =
        htons(port);

    addr.sin_addr.s_addr =
        INADDR_ANY;



    bind(
        listenfd_,
        (sockaddr*)&addr,
        sizeof(addr)
    );


    listen(
        listenfd_,
        128
    );



    channel_ =
        new Channel(
            loop,
            listenfd_
        );


    channel_->setReadCallback(
        [this]()
        {
            handleRead();
        }
    );


    channel_->enableReading();

}




void Acceptor::handleRead()
{

    int clientfd =
        accept(
            listenfd_,
            nullptr,
            nullptr
        );


    /*
        Step 12.3 暂时:

        这里只打印

        后面 Step 12.5
        引入 Connection

    */
    std::cout 
        << "Accepted client fd="
        << clientfd
        << std::endl;

    close(clientfd);

}