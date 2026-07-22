#pragma once

#include <functional>
#include <cstdint>
#include <sys/epoll.h>


class EventLoop;


class Channel
{

public:

    using EventCallback = std::function<void()>;


    Channel(
        EventLoop* loop,
        int fd
    );


    ~Channel() = default;



    // 返回绑定的 fd
    int fd() const;



    // 当前关注的事件
    uint32_t events() const;



    // 设置 epoll 返回的事件
    void setEvents(
        uint32_t events
    );



    // 注册读事件 callback
    void setReadCallback(
        EventCallback cb
    );



    // 注册写事件 callback
    void setWriteCallback(
        EventCallback cb
    );



    // 开启 EPOLLIN 监听
    void enableReading();



    // 开启 EPOLLOUT 监听
    void enableWriting();



    // epoll 事件触发后调用
    void handleEvent();



private:

    EventLoop* loop_;


    int fd_;


    // 当前注册到 epoll 的事件
    uint32_t events_;


    // epoll 返回后的实际事件
    uint32_t revents_;


    EventCallback readCallback_;


    EventCallback writeCallback_;

};