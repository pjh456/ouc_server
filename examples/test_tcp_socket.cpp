#include <socket/tcp_socket.hpp>

#include <unistd.h>
#include <string.h>
#include <string>
#include <iostream>
#include <thread>
#include <chrono>

int main()
{
    using namespace ouc_server::ouc_socket;

    auto server = TCPSocket::create();
    if (!server.bind("127.0.0.1", 8080))
    {
        perror("socket");
        return 1;
    }

    if (!server.listen())
    {
        perror("listen");
        return 1;
    }
    puts("Server listening on port 8080...");

    auto client = server.accept();
    while (client.get_fd() < 0)
    {
        client = server.accept();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    puts("Client conntected");

    while (true)
    {
        char buf[1024];
        ssize_t n = client.recv(buf, sizeof(buf));

        if (n <= 0)
            continue;
        if (!memcmp("exit", buf, 4))
            break;

        std::string data(buf, n);
        std::cout << "Receive data: " << data;

        // client.send("Receive!: ");
        // client.send(buf);
    }

    client.close();
    server.close();
}