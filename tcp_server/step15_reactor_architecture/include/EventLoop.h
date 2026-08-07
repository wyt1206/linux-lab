#pragma once

#include <sys/epoll.h>

#include <functional>
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
    void doPendingTasks();

    int epollfd_;

    int wakeupfd_;

    bool quit_;

    std::mutex mutex_;

    std::queue<Task> pendingTasks_;
};