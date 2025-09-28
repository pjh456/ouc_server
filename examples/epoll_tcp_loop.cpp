#include <epoll/epoll_loop.hpp>

#include <fcntl.h>
#include <iostream>

int set_nonblock(int fd)
{
    int flags = fcntl(fd, F_GETFL /* Get file discriptor state */, 0);
    return fcntl(fd, F_SETFL /* Set new file discriptor */, flags | O_NONBLOCK);
}

int main()
{
    using namespace ouc_server::epoll;

    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0)
    {
        perror("socket");
        return 1;
    }

    int opt = 1;
    setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock_fd, (sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("bind");
        return 1;
    }
    if (listen(sock_fd, SOMAXCONN) < 0)
    {
        perror("listen");
        return 1;
    }
    set_nonblock(sock_fd);

    puts("Server listening on port 8080...");

    EpollLoop loop;

    loop.add_fd(
        sock_fd,
        EPOLLIN,
        [&]()
        {
        while (true) {
            int client_fd = accept(sock_fd, nullptr, nullptr);
            if (client_fd < 0) break;
            set_nonblock(client_fd);
            std::cout << "New client: " << client_fd << "\n";

            loop.add_fd(
                client_fd, 
                EPOLLIN, 
                [&, client_fd]() 
                {
                char buf[1024];
                ssize_t r = read(client_fd, buf, sizeof(buf));
                if (r <= 0) {
                    std::cout << "Client " << client_fd << " disconnected\n";
                    loop.remove_fd(client_fd);
                    close(client_fd);
                    return;
                }
                std::string msg(buf, r);
                std::cout << "Recv: " << msg;
                write(client_fd, buf, r);
            });
        } });

    loop.run();
}