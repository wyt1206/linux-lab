#ifndef EVENT_LOOP_H
#define EVENT_LOOP_H


#include <sys/epoll.h>

class Channel;



class EventLoop
{

public:

    EventLoop();

    ~EventLoop();



    void addChannel(
        Channel* channel,
        uint32_t events
    );



    void modifyChannel(
        Channel* channel,
        uint32_t events
    );



    void removeChannel(
        Channel* channel
    );



    void loop();


private:

    int epoll_fd_;


    static constexpr int MAX_EVENTS = 10;


    epoll_event events_[MAX_EVENTS];

};


#endif