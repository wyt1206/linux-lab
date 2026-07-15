#include "EpollServer.h"

#include <iostream>

#include <unistd.h>

#include <cstring>

#include <netinet/in.h>

#include <sys/socket.h>

#include <fcntl.h>


EpollServer::EpollServer(int port)
    :
    port_(port),
    listen_fd_(-1),
    epoll_fd_(-1)
{

}


EpollServer::~EpollServer()
{
    if(listen_fd_ != -1)
    {
        close(listen_fd_);
    }


    if(epoll_fd_ != -1)
    {
        close(epoll_fd_);
    }
}



void EpollServer::initSocket()
{

    listen_fd_ = socket(
        AF_INET,
        SOCK_STREAM,
        0
    );


    if(listen_fd_ == -1)
    {
        perror("socket");
        exit(1);
    }



    int opt = 1;

    setsockopt(
        listen_fd_,
        SOL_SOCKET,
        SO_REUSEADDR,
        &opt,
        sizeof(opt)
    );



    sockaddr_in addr{};


    addr.sin_family = AF_INET;

    addr.sin_addr.s_addr = INADDR_ANY;

    addr.sin_port = htons(port_);



    if(bind(
        listen_fd_,
        (sockaddr*)&addr,
        sizeof(addr)
    ) < 0)
    {
        perror("bind");
        exit(1);
    }



    if(listen(
        listen_fd_,
        SOMAXCONN
    ) < 0)
    {
        perror("listen");
        exit(1);
    }



    std::cout
        << "Listening on port "
        << port_
        << std::endl;

}



void EpollServer::initEpoll()
{

    epoll_fd_ = epoll_create1(0);


    if(epoll_fd_ == -1)
    {
        perror("epoll_create1");
        exit(1);
    }



    epoll_event event{};


    event.events = EPOLLIN;

    event.data.fd = listen_fd_;



    if(epoll_ctl(
        epoll_fd_,
        EPOLL_CTL_ADD,
        listen_fd_,
        &event
    ) == -1)
    {
        perror("epoll_ctl");
        exit(1);
    }


    std::cout
        << "epoll initialized"
        << std::endl;

}



void EpollServer::acceptConnection()
{

    int client_fd = accept(
        listen_fd_,
        nullptr,
        nullptr
    );


    if(client_fd == -1)
    {
        perror("accept");
        return;
    }



    epoll_event event{};


    event.events = EPOLLIN;


    event.data.fd = client_fd;



    if(epoll_ctl(
        epoll_fd_,
        EPOLL_CTL_ADD,
        client_fd,
        &event
    ) == -1)
    {
        perror("epoll_ctl client");
        close(client_fd);
        return;
    }



    std::cout
        << "New client fd="
        << client_fd
        << std::endl;

}



void EpollServer::start()
{

    initSocket();

    initEpoll();



    while(true)
    {

        int n = epoll_wait(
            epoll_fd_,
            events_,
            MAX_EVENTS,
            -1
        );


        if(n == -1)
        {
            perror("epoll_wait");
            break;
        }



        for(int i=0;i<n;i++)
        {

            int fd = events_[i].data.fd;



            if(fd == listen_fd_)
            {

                acceptConnection();

            }

            else
            {

                std::cout
                    << "Client event fd="
                    << fd
                    << std::endl;

            }

        }

    }

}