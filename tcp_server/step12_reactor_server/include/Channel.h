#pragma once

#include <functional>
#include <cstdint>

class EventLoop;

class Channel
{

public:

    using Callback =
        std::function<void()>;

    Channel(
        EventLoop* loop,
        int fd
    );

    int fd() const;

    uint32_t events() const;

    void setEvents(
        uint32_t events
    );

    void setRevents(
        uint32_t rev
    );

    void enableReading();

    void enableWriting();

    void disableWriting();

    void setReadCallback(
        Callback cb
    );

    void setWriteCallback(
        Callback cb
    );

    void handleEvent();

private:

    EventLoop* loop_;

    int fd_;

    uint32_t events_;

    uint32_t revents_;

    Callback readCallback_;

    Callback writeCallback_;
};