#include <iostream>
#include <cstring>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>


int main() {

    // 1. 创建 socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd < 0) {
        perror("socket");
        return 1;
    }


    // 2. bind
    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = INADDR_ANY;


    if (bind(server_fd,
             (sockaddr*)&server_addr,
             sizeof(server_addr)) < 0) {

        perror("bind");
        return 1;
    }


    // 3. listen
    if (listen(server_fd, 128) < 0) {
        perror("listen");
        return 1;
    }


    std::cout << "Server listening on port 8080\n";


    // 4. accept
    sockaddr_in client_addr{};
    socklen_t client_len = sizeof(client_addr);


    int client_fd = accept(
        server_fd,
        (sockaddr*)&client_addr,
        &client_len
    );


    if (client_fd < 0) {
        perror("accept");
        return 1;
    }


    std::cout << "Client connected!\n";


    // 后面就是 recv/send

    close(client_fd);
    close(server_fd);

    return 0;
}