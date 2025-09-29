#include <socket/tcp_socket.hpp>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

namespace ouc_server
{
    namespace ouc_socket
    {
        TCPSocket::TCPSocket(int fd)
            : listen_fd(fd)
        {
        }

        TCPSocket::TCPSocket(TCPSocket &&other)
            : listen_fd(other.listen_fd)
        {
            other.listen_fd = -1;
        }

        TCPSocket &TCPSocket::operator=(TCPSocket &&other)
        {
            if (this == &other)
                return *this;

            if (listen_fd >= 0)
                close();
            listen_fd = other.listen_fd;
            other.listen_fd = -1;

            return *this;
        }

        TCPSocket TCPSocket::create()
        {
            int fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
            if (fd < 0)
                perror("socket");

            int opt = 1;
            setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
            return TCPSocket(fd);
        }

        bool TCPSocket::bind(const std::string &ip, uint16_t port)
        {
            sockaddr_in addr{};
            addr.sin_family = AF_INET;
            addr.sin_port = htons(port);
            if (inet_pton(AF_INET, ip.c_str(), &addr.sin_addr) != 1)
                return false;
            return ::bind(listen_fd, (sockaddr *)&addr, sizeof(addr)) == 0;
        }

        bool TCPSocket::listen(int backlog) { return ::listen(listen_fd, backlog) == 0; }

        TCPSocket TCPSocket::accept()
        {
            int client_fd = ::accept4(listen_fd, nullptr, nullptr, SOCK_NONBLOCK);
            if (client_fd < 0)
                return TCPSocket();
            return TCPSocket(client_fd);
        }

        bool TCPSocket::close() { return ::close(listen_fd) == 0; }

        ssize_t TCPSocket::send(const char *buf, size_t len) { return ::send(listen_fd, buf, len, 0); }

        ssize_t TCPSocket::send(const char *buf) { return this->send(buf, sizeof(buf)); }

        ssize_t TCPSocket::recv(void *buf, size_t len) { return ::recv(listen_fd, buf, len, 0); }
    }
}