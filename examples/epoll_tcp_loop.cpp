#include <epoll/epoll_loop.hpp>

#include <fcntl.h>
#include <string.h>
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

    EpollLoop loop(4);

    loop.add_fd(
        sock_fd,
        EPOLLIN,
        [&](int fd)
        {
        while (true)
        {
            int client_fd = accept(fd, nullptr, nullptr);
            if (client_fd < 0)
            {
                if (errno == EAGAIN || errno == EWOULDBLOCK) 
                    break;
                perror("accept");
                break;
            }
            set_nonblock(client_fd);

            loop.add_fd(
                client_fd, 
                EPOLLIN, 
                [](int cfd)
                {
                    char buf[4096];
                    ssize_t n = read(cfd, buf, sizeof(buf));
                    write(cfd, buf, n);
                    if ((!memcmp("exit", buf, 4))) 
                    {
                        close(cfd);
                        return;
                    }
                    std::string data(buf, n);
                    std::cout<< "recv:" << data;
            });
    } });

    while (true)
    {
        loop.poll(10);
    }
}