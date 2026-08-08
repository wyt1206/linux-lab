#include <cstring>
#include <iostream>
#include <string>

#include <arpa/inet.h>
#include <unistd.h>

#include <algorithm>
#include <chrono>
#include <mutex>
#include <thread>
#include <vector>

struct BenchmarkConfig
{
    int clients;
    int requests;
};

std::vector<long long> latencies;

std::mutex latencyMutex;

void runClient(int id, const BenchmarkConfig& config);

int main(int argc, char* argv[])
{
    auto start = std::chrono::steady_clock::now();

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
    auto end = std::chrono::steady_clock::now();

    std::sort(latencies.begin(), latencies.end());

    auto p50 = latencies[latencies.size() * 0.50];

    auto p99 = latencies[latencies.size() * 0.99];

    double seconds = std::chrono::duration<double>(end - start).count();

    double throughput = latencies.size() / seconds;

    std::cout << "========== Benchmark Result ==========" << std::endl;

    std::cout << "Requests: " << latencies.size() << std::endl;

    std::cout << "P50 latency(us): " << p50 << std::endl;

    std::cout << "P99 latency(us): " << p99 << std::endl;

    std::cout << "Throughput(req/s): " << throughput << std::endl;
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

        auto requestStart = std::chrono::high_resolution_clock::now();

        send(fd, msg.data(), msg.size(), 0);

        recv(fd, buffer, sizeof(buffer), 0);

        auto requestEnd = std::chrono::high_resolution_clock::now();

        auto latency = std::chrono::duration_cast<std::chrono::microseconds>(
                           requestEnd - requestStart)
                           .count();
        {
            std::lock_guard<std::mutex> lock(latencyMutex);

            latencies.push_back(latency);
        }
    }

    close(fd);
}