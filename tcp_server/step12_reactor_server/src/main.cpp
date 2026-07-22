#include "EventLoop.h"
#include "Acceptor.h"
#include "TcpConnection.h"


#include <memory>


int main()
{

    EventLoop loop;


    Acceptor acceptor(
        &loop,
        8080
    );


    acceptor.setNewConnectionCallback(
        [&](int fd)
        {

            auto conn =
                std::make_shared<TcpConnection>(
                    &loop,
                    fd
                );

        }
    );



    loop.loop();


    return 0;
}