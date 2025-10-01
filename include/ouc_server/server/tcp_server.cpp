#include <server/tcp_server.hpp>

#include <stdexcept>

namespace ouc_server
{
    namespace server
    {
        TCPServer::TCPServer(size_t task_count)
            : server_socket(ouc_server::ouc_socket::TCPSocket::create()),
              tasks(task_count)
        {
        }

        TCPServer::~TCPServer()
        {
            // Gracefully close all client connections before shutting down
            for (auto &[k, v] : clients)
                v.close();

            // Close the listening socket
            server_socket.close();
        }

        bool TCPServer::start(const std::string &ip, uint16_t port)
        {
            // Validate socket
            if (server_socket.get_fd() < 0)
                return false;

            // Bind to IP and port
            if (!server_socket.bind(ip, port))
                return false;

            // Start listening
            if (!server_socket.listen())
                return false;

            // Register listening socket with epoll
            epoll_loop.add_fd(
                server_socket.get_fd(),
                EPOLLIN,
                [this](int)
                {
                    // Accept new client connection when epoll signals readable
                    handle_new_connection();
                });
            return true;
        }

        bool TCPServer::add_fd(int fd, ouc_server::ouc_socket::TCPSocket &&tcp_socket)
        {
            if (fd != tcp_socket.get_fd())
                return false;

            // Check for duplicates or invalid socket
            if (clients.count(fd) || tcp_socket.get_fd() < 0)
                return false;

            // Add client socket to active client map
            clients.emplace(fd, std::move(tcp_socket));

            // Register client socket with epoll to monitor for input events
            if (!epoll_loop.add_fd(
                    fd,
                    EPOLLIN,
                    [this](int fd)
                    {
                        this->handle_client_event(fd);
                    }))
                return false;

            // Fire connection callback if registered
            if (on_connection_callback)
                on_connection_callback(clients.at(fd));

            return true;
        }

        bool TCPServer::add_fd(int fd)
        {
            // Check for duplicates or invalid socket
            if (clients.count(fd) || fd < 0)
                return false;

            // Construct socket from raw fd and reuse add_fd logic
            return add_fd(fd, ouc_server::ouc_socket::TCPSocket(fd));
        }

        bool TCPServer::remove_fd(ouc_server::ouc_socket::TCPSocket &client)
        {
            int fd = client.get_fd();

            // Ensure socket is tracked
            if ((!clients.count(fd)) || fd < 0)
                return false;

            // Remove from epoll monitoring
            if (!epoll_loop.remove_fd(client.get_fd()))
                return false;

            // Fire close callback if registered
            if (on_close_callback)
                on_close_callback(client);

            // Erase from active client list
            clients.erase(client.get_fd());

            // Close underlying socket
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
            // Accept all pending connections in a loop
            while (true)
            {
                auto client = server_socket.accept();

                // Retry if accept failed (could be interrupted)
                while (client.get_fd() < 0)
                    client = server_socket.accept();

                // Add new client to epoll and client map
                add_fd(client.get_fd(), std::move(client));
            }
        }

        void TCPServer::handle_client_event(int fd)
        {
            auto &client = clients[fd];
            char buf[4096];

            // Keep reading until socket would block or closed
            while (true)
            {
                ssize_t n = client.recv(buf, sizeof(buf));
                if (n > 0)
                {
                    // Convert received buffer to string
                    std::string data(buf, n);

                    // Dispatch message callback asynchronously via thread pool
                    if (on_message_callback)
                        tasks.sumbit(on_message_callback, std::ref(client), data);
                }
                else if (n == 0)
                {
                    // n == 0 means peer has closed connection gracefully
                    remove_fd(fd);
                    return;
                }
                else
                {
                    // Handle read error
                    // - EAGAIN / EWOULDBLOCK: no more data available, try again on next epoll event
                    // - EINTR: system call interrupted, retry
                    // - otherwise: critical error, close the connection
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