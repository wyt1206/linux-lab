#pragma once

#include <memory>
#include <unordered_map>

class EventLoop;
class Acceptor;
class TcpConnection;
class ThreadPool;

class TcpServer
{

  public:
    TcpServer(EventLoop* loop, int port);

    ~TcpServer();

    void start();

    void newConnection(int fd);

    void removeConnection(int fd);

  private:
    EventLoop* loop_;

    int port_;

    std::unique_ptr<Acceptor> acceptor_;

    std::unique_ptr<ThreadPool> threadPool_;

    std::unordered_map<int, std::shared_ptr<TcpConnection>> connections_;
};