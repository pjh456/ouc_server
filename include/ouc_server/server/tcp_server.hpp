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
        class TCPServer
        {
        public:
            template <typename... Args>
            using Callback = std::function<void(ouc_server::ouc_socket::TCPSocket &, Args...)>;

        private:
            ouc_server::ouc_socket::TCPSocket server_socket;
            ouc_server::epoll::EpollLoop epoll_loop;
            ouc_server::utils::ThreadPool tasks;
            std::map<int, ouc_server::ouc_socket::TCPSocket> clients;

            Callback<> on_connection_callback;
            Callback<const std::string &> on_message_callback;
            Callback<> on_close_callback;

        public:
            TCPServer();
            ~TCPServer();

        public:
            void on_connection(Callback<> &&callback) { on_connection_callback = std::move(callback); }
            void on_message(Callback<const std::string &> &&callback) { on_message_callback = std::move(callback); }
            void on_close(Callback<> &&callback) { on_close_callback = std::move(callback); }

        public:
            bool start(const std::string &, uint16_t);
            void loop() { epoll_loop.poll(); }

            bool add_fd(int, ouc_server::ouc_socket::TCPSocket &&);
            bool add_fd(int);
            bool remove_fd(ouc_server::ouc_socket::TCPSocket &);
            bool remove_fd(int);

        private:
            void handle_new_connection();
            void handle_client_event(int);
        };
    }
}

#endif // INCLUDE_OUC_SERVER_TCP_SERVER