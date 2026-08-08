#pragma once

#include <cstdint>
#include <functional>

class EventLoop;

enum class ChannelState
{
    NEW,
    ADDED
};

class Channel
{
  public:
    using Callback = std::function<void()>;

    Channel(EventLoop* loop, int fd);

    int fd() const;

    uint32_t events() const;

    void setEvents(uint32_t events);

    void setRevents(uint32_t rev);

    void enableReading();

    void enableWriting();

    void disableWriting();

    void setReadCallback(Callback cb);

    void setWriteCallback(Callback cb);

    void setCloseCallback(Callback cb);

    void handleEvent();

    bool isNew() const;

    void setAdded();

  private:
    EventLoop* loop_;

    int fd_;

    uint32_t events_;

    uint32_t revents_;

    ChannelState state_;

    Callback readCallback_;

    Callback writeCallback_;

    Callback closeCallback_;
};