#include "Server.h"


#include <iostream>


int main()
{

    Server server(8080);


    std::cout
        << "Server started\n";


    server.start();


    return 0;
}