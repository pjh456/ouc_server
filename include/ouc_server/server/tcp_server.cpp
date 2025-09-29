#include <server/tcp_server.hpp>

#include <stdexcept>

namespace ouc_server
{
    namespace server
    {
        TCPServer::TCPServer()
            : server_socket(ouc_server::ouc_socket::TCPSocket::create()),
              tasks(8)
        {
        }

        TCPServer::~TCPServer()
        {
            for (auto &[k, v] : clients)
                v.close();
            server_socket.close();
        }

        bool TCPServer::start(const std::string &ip, uint16_t port)
        {
            if (server_socket.get_fd() < 0)
                return false;
            if (!server_socket.bind(ip, port))
                return false;
            if (!server_socket.listen())
                return false;

            epoll_loop.add_fd(
                server_socket.get_fd(),
                EPOLLIN,
                [this](int)
                {
                    handle_new_connection();
                });
            return true;
        }

        bool TCPServer::add_fd(int fd, ouc_server::ouc_socket::TCPSocket &&tcp_socket)
        {
            if (clients.count(fd) || tcp_socket.get_fd() < 0)
                return false;

            clients.emplace(fd, std::move(tcp_socket));

            if (!epoll_loop.add_fd(
                    fd,
                    EPOLLIN,
                    [this](int fd)
                    {
                        this->handle_client_event(fd);
                    }))
                return false;

            if (on_connection_callback)
                on_connection_callback(clients.at(fd));

            return true;
        }

        bool TCPServer::add_fd(int fd)
        {
            if (clients.count(fd) || fd < 0)
                return false;
            return add_fd(fd, ouc_server::ouc_socket::TCPSocket(fd));
        }

        bool TCPServer::remove_fd(ouc_server::ouc_socket::TCPSocket &client)
        {
            int fd = client.get_fd();
            if ((!clients.count(fd)) || fd < 0)
                return false;

            if (!epoll_loop.remove_fd(client.get_fd()))
                return false;

            if (on_close_callback)
                on_close_callback(client);

            clients.erase(client.get_fd());
            return client.close();
        }

        bool TCPServer::remove_fd(int fd)
        {
            if ((!clients.count(fd)) || fd < 0)
                return false;

            auto &client = clients.at(fd);

            return remove_fd(client);
        }

        void TCPServer::handle_new_connection()
        {
            while (true)
            {
                auto client = server_socket.accept();
                while (client.get_fd() < 0)
                    client = server_socket.accept();

                add_fd(client.get_fd(), std::move(client));
            }
        }

        void TCPServer::handle_client_event(int fd)
        {
            auto &client = clients[fd];
            char buf[4096];
            while (true)
            {
                ssize_t n = client.recv(buf, sizeof(buf));
                if (n > 0)
                {
                    std::string data(buf, n);
                    if (on_message_callback)
                        tasks.sumbit(on_message_callback, std::ref(client), data);
                }
                else if (n == 0)
                {
                    remove_fd(fd);
                    return;
                }
                else
                {
                    if (errno == EAGAIN || errno == EWOULDBLOCK)
                        continue;
                    if (errno == EINTR)
                        continue;

                    remove_fd(fd);
                }
            }
        }
    }
}