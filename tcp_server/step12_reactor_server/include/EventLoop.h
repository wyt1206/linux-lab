#pragma once

#include <sys/epoll.h>
#include <vector>


class Channel;


class EventLoop {

public:

    EventLoop();

    ~EventLoop();


    void loop();


    void updateChannel(Channel* channel);


private:

    int epollfd_;

    static const int MAX_EVENTS = 1024;


    epoll_event events_[MAX_EVENTS];

};