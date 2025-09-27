#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

/* set nonblock socket */
int set_nonblock(int fd)
{
    int flags = fcntl(fd, F_GETFL /* Get file discriptor state */, 0);
    return fcntl(fd, F_SETFL /* Set new file discriptor */, flags | O_NONBLOCK);
}

int main()
{
    int sock_fd = socket(
        AF_INET,     /* IPv4 */
        SOCK_STREAM, /* TCP */
        0 /* File Descriptor */);
    if (sock_fd < 0)
    {
        perror("socket create error");
        return 1;
    }

    int opt = 1;
    setsockopt(
        sock_fd,
        SOL_SOCKET,
        SO_REUSEADDR, /* Allow to bind port muiltiple times */
        &opt,
        sizeof(opt));

    sockaddr_in addr{};                /* Internet socket address */
    addr.sin_family = AF_INET;         /* IPv4 */
    addr.sin_port = htons(8080);       /* Port */
    addr.sin_addr.s_addr = INADDR_ANY; /* listen all */

    if (bind(sock_fd, (sockaddr *)&addr, sizeof(addr))) /* Bind file descriptor with socket address. */
    {
        perror("bind address error");
        return 1;
    }
    if (listen(sock_fd, SOMAXCONN /* Max wait queue that system allows. */)) /* Listen the file descriptor */
    {
        perror("set listen queue error");
        return 1;
    }
    set_nonblock(sock_fd);

    puts("Server listening on port 8080...");

    int epoll_fd = epoll_create1(0); /* create epoll instance */
    epoll_event ev{};                /* create epoll event instance*/
    ev.events = EPOLLIN;             /**/
    ev.data.fd = sock_fd;
    epoll_ctl(
        epoll_fd,
        EPOLL_CTL_ADD, /* Add a file descriptor into epoll to be listened. */
        sock_fd,
        &ev);

    const int MAX_EVENTS = 64;
    epoll_event events[MAX_EVENTS];

    while (true)
    {
        int n = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        for (int i = 0; i < n; ++i)
        {
            int fd = events[i].data.fd;
            if (fd == sock_fd)
            {
                while (true)
                {
                    int client_fd = accept(sock_fd, nullptr, nullptr);
                    if (client_fd < 0)
                        break;
                    printf("New client connected: %d\n", client_fd);
                    set_nonblock(client_fd);

                    epoll_event cli_ev{};
                    cli_ev.events = EPOLLIN; /* Wait for new epoll connection */
                    cli_ev.data.fd = client_fd;
                    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &cli_ev);
                }
            }
            else
            {
                char buf[1024];
                memset(buf, 0, sizeof(buf));
                ssize_t r = read(fd, buf, sizeof(buf));
                write(fd, buf, r); /* Echo data into buffer */

                if (r <= 0 || (!memcmp("exit", buf, 4)))
                {
                    epoll_ctl(
                        epoll_fd,
                        EPOLL_CTL_DEL, /* Delete file descriptor in epoll */
                        fd,
                        nullptr);
                    close(fd);
                    continue;
                }

                printf(buf);
            }
        }
    }

    close(epoll_fd);
    close(sock_fd);
    return 0;
}