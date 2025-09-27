#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

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
    setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

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

    puts("Server listening on port 8080...");

    int client_fd = 0;
    while (true)
    {
        if (client_fd < 0)
        {
            perror("accept socket error");
            continue;
        }
        else if (client_fd == 0)
        {
            client_fd = accept(sock_fd, nullptr, nullptr); /* Accept from client */
            if (client_fd > 0)
                printf("New client connected: %d\n", client_fd);
            continue;
        }

        char buf[1024];
        memset(buf, 0, sizeof(buf));
        ssize_t n = read(client_fd, buf, sizeof(buf)); /* Get data length */
        if (n > 0)
        {
            if (!memcmp("exit", buf, 4))
                break;
            write(client_fd, buf, n); /* Echo data into buffer */
            printf(buf);
        }
    }

    close(client_fd);
    close(sock_fd);
    return 0;
}