#include <iostream>
#include <thread>
#include <cstring>

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>


// 每个 client 的处理函数
void handle_client(int client_fd)
{
    std::cout 
        << "Client handler started. Thread id: "
        << std::this_thread::get_id()
        << std::endl;


    char buffer[1024];


    while (true)
    {
        // 从 client 接收数据
        int bytes_received = recv(
            client_fd,
            buffer,
            sizeof(buffer),
            0
        );


        // client关闭连接
        if (bytes_received <= 0)
        {
            std::cout
                << "Client disconnected. Thread id: "
                << std::this_thread::get_id()
                << std::endl;

            break;
        }


        std::cout << "Received: ";

        std::cout.write(
            buffer,
            bytes_received
        );

        std::cout << std::endl;



        // echo 回 client
        int bytes_sent = send(
            client_fd,
            buffer,
            bytes_received,
            0
        );


        if (bytes_sent < 0)
        {
            perror("send");
            break;
        }
    }


    close(client_fd);
}



int main()
{
    /*
     * Step 1:
     * 创建 socket
     */
    int server_fd = socket(
        AF_INET,
        SOCK_STREAM,
        0
    );


    if (server_fd < 0)
    {
        perror("socket");
        return 1;
    }



    /*
     * Step 2:
     * bind
     */
    sockaddr_in server_addr{};

    server_addr.sin_family = AF_INET;

    server_addr.sin_port = htons(8080);

    server_addr.sin_addr.s_addr = INADDR_ANY;


    if (bind(
            server_fd,
            (sockaddr*)&server_addr,
            sizeof(server_addr)
        ) < 0)
    {
        perror("bind");
        return 1;
    }



    /*
     * Step 3:
     * listen
     */
    if (listen(server_fd, 128) < 0)
    {
        perror("listen");
        return 1;
    }


    std::cout
        << "Server listening on port 8080..."
        << std::endl;



    /*
     * Step 4:
     * accept loop
     */
    while (true)
    {
        sockaddr_in client_addr{};

        socklen_t client_len =
            sizeof(client_addr);



        int client_fd = accept(
            server_fd,
            (sockaddr*)&client_addr,
            &client_len
        );


        if (client_fd < 0)
        {
            perror("accept");
            continue;
        }


        std::cout
            << "New client connected!"
            << std::endl;



        /*
         * Step 7:
         * 一个 client 一个 thread
         */
        std::thread client_thread(
            handle_client,
            client_fd
        );


        /*
         * 让线程后台运行
         */
        client_thread.detach();
    }



    close(server_fd);

    return 0;
}