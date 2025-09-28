#include <epoll/epoll_loop.hpp>

#include <unistd.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <vector>

namespace ouc_server
{
    namespace epoll
    {

        EpollLoop::EpollLoop()
        {
            epoll_fd = epoll_create1(0);
        }

        EpollLoop::~EpollLoop()
        {
            close(epoll_fd);
        }

        void EpollLoop::run(int timeout_ms)
        {
            std::vector<struct epoll_event> events(64);
            while (true)
            {
                int nfds = epoll_wait(epoll_fd, events.data(), events.size(), timeout_ms);
                if (nfds < 0)
                {
                    perror("epoll_wait");
                    break;
                }
                for (int i = 0; i < nfds; ++i)
                {
                    int fd = events[i].data.fd;
                    if (events_map.count(fd))
                    {
                        events_map[fd].callback();
                    }
                }
            }
        }

        bool EpollLoop::add_fd(int fd, uint32_t event_flags, EpollCallback callback)
        {
            auto ev = pack_event(fd, event_flags);
            int code = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev);
            events_map[fd] = Event{fd, event_flags, callback};
            return code == 0;
        }

        bool EpollLoop::modify_fd(int fd, uint32_t event_flags)
        {
            auto ev = pack_event(fd, event_flags);
            int code = epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &ev);
            events_map[fd].events = event_flags;
            return code == 0;
        }

        bool EpollLoop::remove_fd(int fd)
        {
            int code = epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, nullptr);
            events_map.erase(fd);
            return code == 0;
        }

        struct epoll_event EpollLoop::pack_event(int fd, uint32_t flags)
        {
            struct epoll_event ev;
            ev.events = flags;
            ev.data.fd = fd;
            return ev;
        }
    }
}