#pragma once

#include <sys/epoll.h>

#include <functional>
#include <memory>
#include <mutex>
#include <queue>

class Channel;

class EventLoop
{

  public:
    using Task = std::function<void()>;

    EventLoop();

    ~EventLoop();

    void loop();

    void updateChannel(Channel* channel);

    void removeChannel(Channel* channel);

    void queueInLoop(Task task);

  private:
    void wakeup();

    void handleWakeup();

    void doPendingTasks();

    int epollfd_;

    int wakeupfd_;

    bool quit_;

    std::unique_ptr<Channel> wakeupChannel_;

    std::mutex mutex_;

    std::queue<Task> pendingTasks_;
};