#include "EventLoop.h"
#include "Channel.h"

#include <cerrno>
#include <cstdlib>
#include <iostream>
#include <sys/eventfd.h>
#include <unistd.h>

EventLoop::EventLoop()
    : epollfd_(-1), wakeupfd_(-1), quit_(false),
      threadId_(std::this_thread::get_id())
{

    /*
        1. create epoll instance
    */
    epollfd_ = epoll_create1(0);

    if (epollfd_ < 0)
    {
        perror("epoll_create1");
        exit(1);
    }

    /*
        2. create eventfd
    */
    wakeupfd_ = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);

    if (wakeupfd_ < 0)
    {
        perror("eventfd");
        exit(1);
    }

    /*
        3. create Channel for eventfd
    */
    wakeupChannel_ = std::make_unique<Channel>(this, wakeupfd_);

    wakeupChannel_->setReadCallback([this]() { handleWakeup(); });

    /*
        4. register eventfd into epoll
    */
    wakeupChannel_->enableReading();
}

EventLoop::~EventLoop()
{
    if (epollfd_ >= 0)
    {
        close(epollfd_);
    }

    if (wakeupfd_ >= 0)
    {
        close(wakeupfd_);
    }
}

void EventLoop::updateChannel(Channel* channel)
{

    epoll_event ev{};

    ev.events = channel->events();

    /*
    Store Channel pointer

    epoll event returns:
        Channel*
    */
    ev.data.ptr = channel;

    if (channel->isNew())
    {
        /*
            First time registration

            EPOLL_CTL_ADD
        */
        if (epoll_ctl(epollfd_, EPOLL_CTL_ADD, channel->fd(), &ev) < 0)
        {
            perror("epoll_ctl ADD");
            return;
        }

        channel->setAdded();
    }
    else
    {
        /*
        Update existing fd

        EPOLL_CTL_MOD
        */
        if (epoll_ctl(epollfd_, EPOLL_CTL_MOD, channel->fd(), &ev) < 0)
        {
            perror("epoll_ctl MOD");
            return;
        }
    }
}

void EventLoop::removeChannel(Channel* channel)
{
    /*
        Remove fd from epoll

        event parameter ignored for DEL
    */
    if (epoll_ctl(epollfd_, EPOLL_CTL_DEL, channel->fd(), nullptr) < 0)
    {
        perror("epoll_ctl DEL");
    }
}

void EventLoop::loop()
{

    epoll_event events[10];

    while (!quit_)
    {

        int n = epoll_wait(epollfd_, events, 10, -1);

        if (n < 0)
        {
            /*
                Interrupted by signal

                retry
            */
            if (errno == EINTR)
            {
                continue;
            }

            perror("epoll_wait");
            break;
        }

        for (int i = 0; i < n; i++)
        {

            Channel* ch = static_cast<Channel*>(events[i].data.ptr);

            if (ch == nullptr)
            {
                continue;
            }

            ch->setRevents(events[i].events);

            ch->handleEvent();
        }
    }
}

void EventLoop::queueInLoop(Task task)
{

    {
        std::lock_guard<std::mutex> lock(mutex_);

        pendingTasks_.push(std::move(task));
    }

    wakeup();
}

void EventLoop::doPendingTasks()
{
    std::queue<Task> tasks;

    {
        std::lock_guard<std::mutex> lock(mutex_);

        tasks.swap(pendingTasks_);
    }

    while (!tasks.empty())
    {
        tasks.front()();

        tasks.pop();
    }
}

void EventLoop::wakeup()
{
    uint64_t one = 1;

    ssize_t n = write(wakeupfd_, &one, sizeof(one));

    if (n != sizeof(one))
    {
        perror("eventfd write");
    }
}

void EventLoop::handleWakeup()
{
    uint64_t one;

    ssize_t n = read(wakeupfd_, &one, sizeof(one));

    if (n != sizeof(one))
    {
        perror("eventfd read");
    }

    doPendingTasks();
}

bool EventLoop::isInLoopThread()
{
    return threadId_ == std::this_thread::get_id();
}

void EventLoop::runInLoop(Task task)
{
    if (isInLoopThread())
    {
        task();
    }
    else
    {
        queueInLoop(task);
    }
}