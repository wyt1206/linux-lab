#pragma once

#include <memory>
#include <string>

class EventLoop;
class Channel;
class TcpServer;

class TcpConnection : public std::enable_shared_from_this<TcpConnection>
{

  public:
    TcpConnection(EventLoop* loop, TcpServer* server, int fd);

    ~TcpConnection();

    int fd() const;

    void send(const std::string& msg);

  private:
    void handleRead();

    void handleWrite();

    void handleClose();

  private:
    EventLoop* loop_;

    TcpServer* server_;

    int fd_;

    std::unique_ptr<Channel> channel_;

    std::string writeBuffer_;
};