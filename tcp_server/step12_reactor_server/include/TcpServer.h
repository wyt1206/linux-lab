#pragma once

#include <memory>
#include <unordered_map>

class EventLoop;
class Acceptor;
class TcpConnection;

class TcpServer
{

  public:
    TcpServer(EventLoop* loop, int port);

    ~TcpServer();

    void start();

    void removeConnection(int fd);

  private:
    void newConnection(int fd);

  private:
    EventLoop* loop_;

    std::unique_ptr<Acceptor> acceptor_;

    std::unordered_map<int, std::shared_ptr<TcpConnection>> connections_;
};