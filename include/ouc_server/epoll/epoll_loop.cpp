#include <epoll/epoll_loop.hpp>

#include <unistd.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>

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

        bool EpollLoop::add_fd(int fd, uint32_t event_flags, std::function<void()> callback)
        {
            struct epoll_event ev;
            ev.events = event_flags;
            ev.data.fd = fd;
            epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev);
            events_map[fd] = Event{fd, event_flags, callback};
        }
    }
}