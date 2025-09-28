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
        struct Event
        {
            int fd;
            uint32_t events;
            std::function<void()> callback;
        };

        class EpollLoop
        {
        private:
            int epoll_fd;
            std::unordered_map<int, Event> events_map;

        public:
            EpollLoop();

            ~EpollLoop();

        public:
            bool add_fd(int, uint32_t, std::function<void()>);
        };
    }
}

#endif // INCLUDE_OUC_SERVER_EPOLL_LOOP