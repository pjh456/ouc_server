/**
 * @file tcp_server.hpp
 * @brief TCP server interface definition module.
 *
 * This header defines the TCPServer class which provides a wrapper around
 * TCP socket handling, epoll-based event loop, and thread pool task execution.
 * It supports connection, message, and close callbacks for handling events
 * in an asynchronous manner.
 *
 * @author pjh456
 * @date 2025-10-01
 */

#ifndef INCLUDE_OUC_SERVER_TCP_SERVER
#define INCLUDE_OUC_SERVER_TCP_SERVER

#include <cstdint>
#include <string>
#include <functional>
#include <utility>
#include <map>

#include <socket/tcp_socket.hpp>
#include <epoll/epoll_loop.hpp>
#include <utils/thread_pool.hpp>

namespace ouc_server
{
    namespace server
    {
        /**
         * @class TCPServer
         * @brief A TCP server wrapper with epoll-based I/O and callback support.
         *
         * This class manages a TCP listening socket, handles new client
         * connections and client I/O events via epoll, and dispatches
         * user-defined callbacks for connection, message reception, and close events.
         *
         * Example:
         * @code
         * TCPServer server;
         * server.on_connection([](TCPSocket &sock){ ... });
         * server.on_message([](TCPSocket &sock, const std::string &msg){ ... });
         * server.start("127.0.0.1", 8080);
         * while(true) server.loop();
         * @endcode
         */
        class TCPServer
        {
        public:
            template <typename... Args>
            using Callback = std::function<void(ouc_server::ouc_socket::TCPSocket &, Args...)>;

        private:
            ouc_server::ouc_socket::TCPSocket server_socket;          ///< Listening socket for the server.
            ouc_server::epoll::EpollLoop epoll_loop;                  ///< Epoll event loop instance.
            ouc_server::utils::ThreadPool tasks;                      ///< Thread pool for async tasks.
            std::map<int, ouc_server::ouc_socket::TCPSocket> clients; ///< Active client sockets.

            Callback<> on_connection_callback;                 ///< Callback for new connection event.
            Callback<const std::string &> on_message_callback; ///< Callback for message received event.
            Callback<> on_close_callback;                      ///< Callback for client close event.

        public:
            /**
             * @brief Construct a new TCPServer instance.
             * @param task_count Number of thread in the thread pool.
             */
            TCPServer(size_t task_count = 64);

            /**
             * @brief Destroy the TCPServer instance.
             */
            ~TCPServer();

        public:
            /**
             * @brief Register callback for new connections.
             * @param callback Function to call when a client connects.
             */
            void on_connection(Callback<> &&callback) { on_connection_callback = std::move(callback); }

            /**
             * @brief Register callback for incoming messages.
             * @param callback Function to call when a message is received.
             */
            void on_message(Callback<const std::string &> &&callback) { on_message_callback = std::move(callback); }

            /**
             * @brief Register callback for client close events.
             * @param callback Function to call when a client disconnects.
             */
            void on_close(Callback<> &&callback) { on_close_callback = std::move(callback); }

        public:
            /**
             * @brief Start the server listening.
             * @param address IP address to bind.
             * @param port Port number to bind.
             * @return true if the server started successfully, false otherwise.
             */
            bool start(const std::string &address, uint16_t port);

            /**
             * @brief Run the event loop for one time.
             */
            void loop() { epoll_loop.poll(); }

            /**
             * @brief Add a client socket by file descriptor and socket object.
             * @param fd File descriptor.
             * @param socket TCP socket to associate.
             * @return true if added successfully.
             */
            bool add_fd(int fd, ouc_server::ouc_socket::TCPSocket &&p_socket);

            /**
             * @brief Add a client socket by file descriptor only.
             * @param fd File descriptor.
             * @return true if added successfully.
             */
            bool add_fd(int fd);

            /**
             * @brief Remove a client socket by socket object.
             * @param socket TCP socket reference.
             * @return true if removed successfully.
             */
            bool remove_fd(ouc_server::ouc_socket::TCPSocket &p_socket);

            /**
             * @brief Remove a client socket by file descriptor.
             * @param fd File descriptor.
             * @return true if removed successfully.
             */
            bool remove_fd(int);

        private:
            /**
             * @brief Handle a new client connection.
             */
            void handle_new_connection();

            /**
             * @brief Handle an event from a specific client.
             * @param fd File descriptor of the client.
             */
            void handle_client_event(int);
        };
    }
}

#endif // INCLUDE_OUC_SERVER_TCP_SERVER