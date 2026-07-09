#include <iostream>
#include <sys/socket.h>
#include <unistd.h>

int main() {

    // Create a TCP socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd == -1) {
        std::cerr << "Failed to create socket" << std::endl;
        return 1;
    }

    std::cout << "Socket created successfully!" << std::endl;
    std::cout << "Socket FD: " << server_fd << std::endl;


    // Close socket
    close(server_fd);

    return 0;
}