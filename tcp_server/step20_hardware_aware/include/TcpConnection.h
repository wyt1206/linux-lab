#pragma once

#include <memory>
#include <string>

class EventLoop;
class Channel;
class TcpServer;
class ThreadPool;

class TcpConnection : public std::enable_shared_from_this<TcpConnection>
{

  public:
    enum class ConnectionState
    {
        CONNECTING,
        CONNECTED,
        DISCONNECTING,
        DISCONNECTED
    };

    TcpConnection(EventLoop* loop, TcpServer* server, ThreadPool* pool, int fd);

    ~TcpConnection();

    int fd() const;

    void connectEstablished();

    void send(std::string msg);

    bool connected() const;

  private:
    void handleRead();

    void handleWrite();

    void handleClose();

  private:
    EventLoop* loop_;

    TcpServer* server_;

    ThreadPool* threadPool_;

    int fd_;

    std::unique_ptr<Channel> channel_;

    std::string writeBuffer_;

    ConnectionState state_;
};
