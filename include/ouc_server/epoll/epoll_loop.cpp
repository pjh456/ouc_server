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

        EpollLoop::EpollLoop(size_t n)
            : pool(n)
        {
            epoll_fd = epoll_create1(0);
            if (epoll_fd < 0)
                perror("epoll_create1");
        }

        EpollLoop::~EpollLoop()
        {
            close(epoll_fd);
        }

        void EpollLoop::poll(const int timeout_ms, const int MAX_EVENTS)
        {
            epoll_event events[MAX_EVENTS];

            int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, timeout_ms);
            if (nfds < 0)
            {
                if (errno == EINTR)
                    return;
                perror("epoll_wait");
                return;
            }

            for (int i = 0; i < nfds; ++i)
            {
                int fd = events[i].data.fd;
                if (callbacks.count(fd))
                {
                    pool.sumbit(
                        [ev = callbacks[fd], fd]()
                        { ev.callback(fd); });
                }
            }
        }

        bool EpollLoop::add_fd(int fd, uint32_t event_flags, EpollCallback callback)
        {
            auto ev = pack_event(fd, event_flags);
            int code = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev);
            callbacks[fd] = Event{fd, event_flags, std::move(callback)};
            return code == 0;
        }

        bool EpollLoop::modify_fd(int fd, uint32_t event_flags)
        {
            auto ev = pack_event(fd, event_flags);
            int code = epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &ev);
            callbacks[fd].events = event_flags;
            return code == 0;
        }

        bool EpollLoop::remove_fd(int fd)
        {
            int code = epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, nullptr);
            callbacks.erase(fd);
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