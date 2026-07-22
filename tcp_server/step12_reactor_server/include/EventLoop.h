#pragma once

#include <sys/epoll.h>
#include <unordered_map>


class Channel;


class EventLoop
{

public:

    EventLoop();

    ~EventLoop();


    void loop();


    void updateChannel(Channel* channel);


    void removeChannel(Channel* channel);



private:

    int epollfd_;

    bool quit_;

};