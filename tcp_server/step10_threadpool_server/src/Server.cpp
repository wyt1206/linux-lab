#include "Server.h"

#include <iostream>

#include <cstring>

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

Server::Server(int port, int thread_num)
    : port_(port), listen_fd_(-1), pool_(thread_num)
{
}

Server::~Server()
{

    if (listen_fd_ != -1)
    {
        close(listen_fd_);
    }
}

void Server::start()
{

    listen_fd_ = socket(AF_INET, SOCK_STREAM, 0);

    if (listen_fd_ == -1)
    {
        throw std::runtime_error("socket failed");
    }

    sockaddr_in server_addr{};

    server_addr.sin_family = AF_INET;

    server_addr.sin_addr.s_addr = INADDR_ANY;

    server_addr.sin_port = htons(port_);

    if (bind(listen_fd_, (sockaddr*)&server_addr, sizeof(server_addr)) < 0)
    {
        throw std::runtime_error("bind failed");
    }

    if (listen(listen_fd_, 128) < 0)
    {
        throw std::runtime_error("listen failed");
    }

    std::cout << "Server listening on " << port_ << std::endl;

    while (true)
    {

        int client_fd = accept(listen_fd_, nullptr, nullptr);

        if (client_fd < 0)
        {
            continue;
        }

        std::cout << "New client connected: " << client_fd << std::endl;

        pool_.enqueue([this, client_fd]() { handleClient(client_fd); });
    }
}

void Server::handleClient(int client_fd)
{

    char buffer[1024];

    while (true)
    {

        memset(buffer, 0, sizeof(buffer));

        int n = recv(client_fd, buffer, sizeof(buffer), 0);

        if (n <= 0)
        {
            break;
        }

        std::cout << "Received: " << buffer << std::endl;

        send(client_fd, buffer, n, 0);
    }

    std::cout << "Client disconnected: " << client_fd << std::endl;

    close(client_fd);
}