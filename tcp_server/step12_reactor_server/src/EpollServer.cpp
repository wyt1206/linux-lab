#include "EpollServer.h"

#include "Channel.h"


#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>


#include <iostream>
#include <cstring>
#include <cerrno>





EpollServer::EpollServer(
    int port
)
:
port_(port),
listen_fd_(-1)
{

}






EpollServer::~EpollServer()
{

    close(listen_fd_);

}







void EpollServer::setNonBlocking(
    int fd
)
{

    int flags =
        fcntl(
            fd,
            F_GETFL,
            0
        );


    fcntl(
        fd,
        F_SETFL,
        flags | O_NONBLOCK
    );

}








void EpollServer::initSocket()
{

    listen_fd_ =
        socket(
            AF_INET,
            SOCK_STREAM,
            0
        );


    int opt=1;


    setsockopt(
        listen_fd_,
        SOL_SOCKET,
        SO_REUSEADDR,
        &opt,
        sizeof(opt)
    );



    setNonBlocking(
        listen_fd_
    );



    sockaddr_in addr{};


    addr.sin_family =
        AF_INET;


    addr.sin_addr.s_addr =
        INADDR_ANY;


    addr.sin_port =
        htons(port_);




    bind(
        listen_fd_,
        (sockaddr*)&addr,
        sizeof(addr)
    );



    listen(
        listen_fd_,
        128
    );



    listenChannel_ =
        std::make_unique<Channel>(
            listen_fd_
        );



    listenChannel_->setReadCallback(
        [this]()
        {
            acceptConnection();
        }
    );



    loop_.addChannel(
        listenChannel_.get(),
        EPOLLIN | EPOLLET
    );



    std::cout
        << "listen "
        << port_
        << std::endl;

}









void EpollServer::acceptConnection()
{

    while(true)
    {

        int client =
            accept(
                listen_fd_,
                nullptr,
                nullptr
            );



        if(client <0)
        {

            if(errno==EAGAIN)
                break;


            return;

        }



        setNonBlocking(client);



        auto channel =
            std::make_unique<Channel>(
                client
            );



        channel->setReadCallback(
            [this,client]()
            {
                handleRead(client);
            }
        );



        channels_[client] =
            std::move(channel);



        loop_.addChannel(
            channels_[client].get(),
            EPOLLIN | EPOLLET
        );



        std::cout
            << "client "
            << client
            << std::endl;


    }

}








void EpollServer::handleRead(
    int fd
)
{

    char buf[1024];


    int n =
        recv(
            fd,
            buf,
            sizeof(buf),
            0
        );


    if(n>0)
    {

        send(
            fd,
            buf,
            n,
            0
        );

    }

    else
    {

        close(fd);

        channels_.erase(fd);

    }

}







void EpollServer::handleWrite(
    int fd
)
{


}







void EpollServer::start()
{

    initSocket();


    loop_.loop();

}