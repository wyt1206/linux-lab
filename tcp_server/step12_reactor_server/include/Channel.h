#ifndef CHANNEL_H
#define CHANNEL_H

#include <functional>
#include <cstdint>


class Channel
{

public:

    using Callback = std::function<void()>;


    explicit Channel(int fd);


    int fd() const;


    void setReadCallback(
        Callback cb
    );


    void setWriteCallback(
        Callback cb
    );


    void handleEvent(
        uint32_t events
    );


private:

    int fd_;

    Callback readCallback_;

    Callback writeCallback_;

};


#endif