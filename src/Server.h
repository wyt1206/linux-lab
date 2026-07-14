#ifndef SERVER_H
#define SERVER_H


#include "ThreadPool.h"


class Server
{

public:

    Server(
        int port,
        int thread_num
    );


    ~Server();


    void start();



private:

    int port_;

    int listen_fd_;


    ThreadPool pool_;



    void handleClient(
        int client_fd
    );


};


#endif