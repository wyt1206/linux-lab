#include "Server.h"

#include "EventLoop.h"
#include "Acceptor.h"



Server::Server(
    int port
)
{

    loop_=new EventLoop();


    acceptor_ =
        new Acceptor(
            loop_,
            port
        );

}



void Server::start()
{

    loop_->loop();

}