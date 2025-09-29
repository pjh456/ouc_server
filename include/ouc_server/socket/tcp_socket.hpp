#ifndef INCLUDE_OUC_SERVER_TCP_SOCKET
#define INCLUDE_OUC_SERVER_TCP_SOCKET

#include <cstdint>
#include <string>

namespace ouc_server
{
    namespace ouc_socket
    {
        class TCPSocket
        {
        private:
            int listen_fd;

        public:
            explicit TCPSocket(int = -1);

            TCPSocket(const TCPSocket &) = delete;
            TCPSocket &operator=(const TCPSocket &) = delete;

            TCPSocket(TCPSocket &&);
            TCPSocket &operator=(TCPSocket &&);

            static TCPSocket create();

        public:
            int get_fd() const { return listen_fd; }

            bool bind(const std::string &, uint16_t);
            bool listen(int = 128);
            TCPSocket accept();
            bool close();

        public:
            ssize_t send(const char *, size_t);
            ssize_t send(const char *);
            ssize_t recv(void *, size_t);
        };
    }
}

#endif // INCLUDE_OUC_SERVER_TCP_SOCKET