#include <cstring>
#include <iostream>
#include <string>

#include <arpa/inet.h>
#include <unistd.h>

#include <thread>
#include <vector>

struct BenchmarkConfig
{
    int clients;
    int requests;
};

void runClient(int id, const BenchmarkConfig& config);

int main(int argc, char* argv[])
{

    BenchmarkConfig config;

    config.clients = 10;
    config.requests = 100;

    if (argc >= 3)
    {
        config.clients = std::stoi(argv[1]);
        config.requests = std::stoi(argv[2]);
    }

    std::vector<std::thread> threads;

    for (int i = 0; i < config.clients; i++)
    {
        threads.emplace_back(runClient, i, std::ref(config));
    }

    for (auto& t : threads)
    {
        t.join();
    }
}

void runClient(int id, const BenchmarkConfig& config)
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in server{};

    server.sin_family = AF_INET;
    server.sin_port = htons(8080);

    connect(fd, (sockaddr*)&server, sizeof(server));

    char buffer[4096];

    for (int i = 0; i < config.requests; i++)
    {

        std::string msg =
            "client-" + std::to_string(id) + "-request-" + std::to_string(i);

        send(fd, msg.data(), msg.size(), 0);

        recv(fd, buffer, sizeof(buffer), 0);
    }

    close(fd);
}