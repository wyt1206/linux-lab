#include "EpollServer.h"



#include <iostream>

#include <unistd.h>

#include <fcntl.h>

#include <cstring>

#include <cerrno>


#include <netinet/in.h>

#include <sys/socket.h>






EpollServer::EpollServer(int port)
    :
    port_(port),
    listen_fd_(-1)
{

}







EpollServer::~EpollServer()
{

    for(auto &c : connections_)
    {
        close(c.first);
    }


    if(listen_fd_ >= 0)
    {
        close(listen_fd_);
    }

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



    int opt = 1;


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
        SOMAXCONN
    );



    loop_.addFd(
        listen_fd_,
        EPOLLIN | EPOLLET
    );



    std::cout
        << "server listen "
        << port_
        << std::endl;

}









void EpollServer::acceptConnection()
{

    while(true)
    {


        int client_fd =
            accept(
                listen_fd_,
                nullptr,
                nullptr
            );



        if(client_fd < 0)
        {

            if(errno == EAGAIN ||
               errno == EWOULDBLOCK)
            {
                break;
            }


            break;

        }



        setNonBlocking(
            client_fd
        );



        connections_[client_fd]
        =
        {
            client_fd,
            ""
        };



        loop_.addFd(
            client_fd,
            EPOLLIN | EPOLLET
        );



        std::cout
            << "client "
            << client_fd
            << std::endl;


    }

}









void EpollServer::handleRead(
    int fd
)
{

    char buffer[1024];



    while(true)
    {


        int n =
            recv(
                fd,
                buffer,
                sizeof(buffer),
                0
            );



        if(n > 0)
        {

            connections_[fd]
                .write_buffer
                .append(
                    buffer,
                    n
                );

        }


        else if(n == 0)
        {

            closeConnection(fd);

            break;

        }


        else
        {

            if(errno == EAGAIN ||
               errno == EWOULDBLOCK)
            {
                break;
            }


            closeConnection(fd);

            break;

        }


    }



    if(
        connections_.count(fd)
        &&
        !connections_[fd]
        .write_buffer.empty()
    )
    {

        enableWrite(fd);

    }

}









void EpollServer::handleWrite(
    int fd
)
{

    auto &buffer =
        connections_[fd]
        .write_buffer;



    while(!buffer.empty())
    {


        int n =
            send(
                fd,
                buffer.data(),
                buffer.size(),
                0
            );


        if(n > 0)
        {

            buffer.erase(
                0,
                n
            );

        }


        else
        {

            if(errno == EAGAIN ||
               errno == EWOULDBLOCK)
            {
                break;
            }


            closeConnection(fd);

            return;

        }

    }



    if(buffer.empty())
    {
        disableWrite(fd);
    }

}









void EpollServer::enableWrite(
    int fd
)
{

    loop_.modifyFd(
        fd,
        EPOLLIN |
        EPOLLOUT |
        EPOLLET
    );

}








void EpollServer::disableWrite(
    int fd
)
{

    loop_.modifyFd(
        fd,
        EPOLLIN |
        EPOLLET
    );

}








void EpollServer::closeConnection(
    int fd
)
{

    loop_.removeFd(fd);


    close(fd);


    connections_.erase(fd);

}









void EpollServer::start()
{

    initSocket();



    while(true)
    {


        int n =
            loop_.wait(
                events_,
                MAX_EVENTS
            );



        for(int i=0;i<n;i++)
        {


            int fd =
                events_[i]
                .data.fd;



            uint32_t event =
                events_[i]
                .events;



            if(fd == listen_fd_)
            {

                acceptConnection();

            }

            else
            {


                if(event & EPOLLIN)
                {
                    handleRead(fd);
                }



                if(event & EPOLLOUT)
                {
                    handleWrite(fd);
                }

            }


        }


    }


}