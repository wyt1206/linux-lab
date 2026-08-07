#pragma once

#include <functional>
#include <memory>

class EventLoop;
class Channel;

class Acceptor
{

  public:
    using NewConnectionCallback = std::function<void(int)>;

    Acceptor(EventLoop* loop, int port);

    ~Acceptor();

    void setNewConnectionCallback(NewConnectionCallback cb);

  private:
    void handleRead();

  private:
    EventLoop* loop_;

    int listenfd_;

    std::unique_ptr<Channel> channel_;

    NewConnectionCallback callback_;
};