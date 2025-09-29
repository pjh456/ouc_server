#include <server/tcp_server.hpp>

#include <unistd.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <string.h>

int main()
{
    using namespace ouc_server::server;
    using namespace ouc_server::ouc_socket;

    TCPServer server;
    if (!server.start("127.0.0.1", 8080))
    {
        std::cerr << "Failed to start server\n";
        return 1;
    }

    puts("Start Listening!");

    server.on_connection(
        [](TCPSocket &client)
        { std::cout << "New client connected, fd=" << client.get_fd() << "\n"; });

    server.on_message(
        [&](TCPSocket &client, const std::string &msg)
        {
            if (!memcmp("exit", msg.c_str(), 4))
            {
                server.remove_fd(client);
                return;
            }
            std::cout << "Received: " << msg;

            std::string ret_str("Echo: " + msg);
            size_t count = client.send(ret_str.c_str());
            while (count < ret_str.size())
                count += client.send(ret_str.c_str() + count);
        });

    server.on_close(
        [](TCPSocket &client)
        { std::cout << "Client disconnected, fd=" << client.get_fd() << "\n"; });

    while (true)
    {
        server.loop();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}
