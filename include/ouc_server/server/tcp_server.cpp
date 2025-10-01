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

        TCPServer::~TCPServer() noexcept
        {
            // Destructor must never throw.
            // Wrap all potentially throwing operations in try/catch.
            try
            {
                for (auto &[k, v] : clients)
                {
                    try
                    {
                        v.close(); // ensure socket closed
                    }
                    catch (...)
                    {
                        // swallow to keep noexcept guarantee
                    }
                }

                try
                {
                    server_socket.close();
                }
                catch (...)
                {
                    // swallow to keep noexcept guarantee
                }
            }
            catch (...)
            {
                // Final catch-all, should never happen
            }
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
            // Important: wrap callback in try/catch to prevent exception
            // escaping into epoll loop.
            return epoll_loop.add_fd(
                server_socket.get_fd(),
                EPOLLIN,
                [this](int)
                {
                    try
                    {
                        handle_new_connection();
                    }
                    catch (const std::exception &e)
                    {
                        // continue server loop, ignore this event
                    }
                    catch (...)
                    {
                        // unknown error happened
                    }
                });
        }

        bool TCPServer::add_fd(int fd, ouc_server::ouc_socket::TCPSocket &&tcp_socket)
        {
            if (fd != tcp_socket.get_fd())
                return false;

            // Check for duplicates or invalid socket
            if (clients.count(fd) || tcp_socket.get_fd() < 0)
                return false;

            // First try adding to epoll
            if (!epoll_loop.add_fd(
                    fd,
                    EPOLLIN,
                    [this](int fd)
                    {
                        this->handle_client_event(fd);
                    }))
            {
                return false;
            }

            // Use temporary to ensure strong exception safety:
            // if emplace throws, tmp will be destroyed properly.
            ouc_server::ouc_socket::TCPSocket temp = std::move(tcp_socket);

            // Only insert into clients if epoll registration succeeded
            auto [it, inserted] = clients.emplace(fd, std::move(temp));
            if (!inserted)
            {
                // Rollback epoll registration if insertion failed
                epoll_loop.remove_fd(fd);
                return false;
            }

            if (on_connection_callback)
            {
                try
                {
                    on_connection_callback(it->second);
                }
                catch (...)
                {
                    // Rollback if callback throws
                    epoll_loop.remove_fd(fd);
                    clients.erase(it);
                    // Do NOT rethrow: keep server stable
                }
            }

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

            // Always erase from map first to keep internal state consistent
            auto it = clients.find(fd);
            if (it == clients.end())
                return false;

            auto rm_socket = std::move(it->second);
            clients.erase(it);

            // Try to remove from epoll
            if (!epoll_loop.remove_fd(fd))
            {
                // Best effort rollback: not fatal, but we already removed from map
                // maybe log warning here
            }

            // Safe to close now
            bool closed = rm_socket.close();

            // Callback is external, wrap in try/catch to not break server loop
            if (on_close_callback)
            {
                try
                {
                    on_close_callback(rm_socket);
                }
                catch (...)
                {
                    // Log error, swallow exception to keep server stable
                }
            }

            return closed;
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