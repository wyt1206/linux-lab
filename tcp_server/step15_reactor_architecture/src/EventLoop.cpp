#include "EventLoop.h"
#include "Channel.h"

#include <cerrno>
#include <cstdlib>
#include <iostream>
#include <unistd.h>

EventLoop::EventLoop() : epollfd_(-1), wakeupfd_(-1), quit_(false)
{
    epollfd_ = epoll_create1(0);

    if (epollfd_ < 0)
    {
        perror("epoll_create1");
        exit(1);
    }
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
    std::lock_guard<std::mutex> lock(mutex_);

    pendingTasks_.push(std::move(task));
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