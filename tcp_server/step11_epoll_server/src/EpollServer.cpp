#include "EpollServer.h"


#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <cstring>
#include <cstdlib>



EpollServer::EpollServer(int port)
    :
      port_(port),
      server_fd_(-1),
      epoll_fd_(-1),
      running_(true)
{

}



EpollServer::~EpollServer()
{

    if(server_fd_ >= 0)
    {
        close(server_fd_);
    }


    if(epoll_fd_ >= 0)
    {
        close(epoll_fd_);
    }

}



void EpollServer::start()
{

    setupSocket();


    setupEpoll();


    eventLoop();

}



void EpollServer::setupSocket()
{

    server_fd_ = socket(
        AF_INET,
        SOCK_STREAM,
        0
    );


    if(server_fd_ < 0)
    {
        perror("socket");
        exit(1);
    }



    int opt = 1;


    setsockopt(
        server_fd_,
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
        server_fd_,
        (sockaddr*)&addr,
        sizeof(addr)
    ) < 0)
    {
        perror("bind");
        exit(1);
    }



    if(listen(server_fd_, 128) < 0)
    {
        perror("listen");
        exit(1);
    }



    std::cout
        << "Server listening on port "
        << port_
        << std::endl;

}





void EpollServer::setupEpoll()
{

    /*
        创建 epoll instance

    */

    epoll_fd_ = epoll_create1(0);



    if(epoll_fd_ < 0)
    {
        perror("epoll_create1");
        exit(1);
    }



    /*
        注册 server socket

        监听:

        EPOLLIN

    */


    epoll_event event{};


    event.events = EPOLLIN;


    event.data.fd = server_fd_;



    if(epoll_ctl(
        epoll_fd_,
        EPOLL_CTL_ADD,
        server_fd_,
        &event
    ) < 0)
    {
        perror("epoll_ctl");
        exit(1);
    }



    std::cout
        << "epoll setup complete\n";

}





void EpollServer::eventLoop()
{

    constexpr int MAX_EVENTS = 10;


    epoll_event events[MAX_EVENTS];



    while(running_)
    {

        int n = epoll_wait(
            epoll_fd_,
            events,
            MAX_EVENTS,
            -1
        );



        if(n < 0)
        {
            perror("epoll_wait");
            break;
        }



        for(int i = 0; i < n; i++)
        {

            int fd = events[i].data.fd;



            if(fd == server_fd_)
            {

                acceptConnection();

            }

        }

    }

}





void EpollServer::acceptConnection()
{

    sockaddr_in client_addr{};


    socklen_t len = sizeof(client_addr);



    int client_fd = accept(
        server_fd_,
        (sockaddr*)&client_addr,
        &len
    );



    if(client_fd < 0)
    {
        perror("accept");
        return;
    }



    std::cout
        << "New client connected. fd="
        << client_fd
        << std::endl;



    /*
        Step 11.2 到这里结束

        下一步：

        epoll_ctl ADD client_fd

    */

}