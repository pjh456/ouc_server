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

namespace ouc_server
{
    namespace epoll
    {
        using EpollCallback = std::function<void()>;

        struct Event
        {
            int fd;
            uint32_t events;
            EpollCallback callback;
        };

        class EpollLoop
        {
        public:
        private:
            int epoll_fd;
            std::unordered_map<int, Event> events_map;

        public:
            EpollLoop();

            ~EpollLoop();

        public:
            void run(int = -1);

            bool add_fd(int, uint32_t, EpollCallback);
            bool modify_fd(int, uint32_t);
            bool remove_fd(int);

        private:
            struct epoll_event pack_event(int, uint32_t);
        };
    }
}

#endif // INCLUDE_OUC_SERVER_EPOLL_LOOP