#ifndef INCLUDE_OUC_SERVER_EPOLL_LOOP
#define INCLUDE_OUC_SERVER_EPOLL_LOOP

#include <cstdint>
#include <string>
#include <functional>
#include <unordered_map>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>

#include <utils/thread_pool.hpp>

namespace ouc_server
{
    namespace epoll
    {
        using EpollCallback = std::function<void(int)>;

        struct Event
        {
            int fd;
            uint32_t events;
            EpollCallback callback;
        };

        class EpollLoop
        {
        private:
            int epoll_fd;
            std::unordered_map<int, Event> callbacks;

            ouc_server::utils::ThreadPool pool;

        public:
            EpollLoop(size_t = 64);

            ~EpollLoop();

        public:
            void poll(const int = 0, const int = 64);

            bool add_fd(int, uint32_t, EpollCallback);
            bool modify_fd(int, uint32_t);
            bool remove_fd(int);

        private:
            struct epoll_event pack_event(int, uint32_t);
        };
    }
}

#endif // INCLUDE_OUC_SERVER_EPOLL_LOOP