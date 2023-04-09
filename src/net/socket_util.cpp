/*************************************************************************
Copyright (C) 2023 The xfnet Authors. All rights reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
***************************************************************************/

#include <poll.h>
#include <fcntl.h>
#include <error.h>
#include "xfnet.h"

#define REUSEPORT 1

using namespace xfutil;

namespace xfnet 
{
    
const int SocketUtil::EVENT_READ = POLLIN | POLLPRI;
const int SocketUtil::EVENT_WRITE = POLLOUT;
const int SocketUtil::EVENT_READWRITE = SocketUtil::EVENT_READ | SocketUtil::EVENT_WRITE;

int SocketUtil::Open(int family/* = AF_INET*/)
{
    int fd = socket(family, SOCK_STREAM|SOCK_NONBLOCK|SOCK_CLOEXEC, IPPROTO_TCP);
    if(fd != INVALID_SOCKET)
    {
        SetNoDelay(fd, true);
        SetKeepAlive(fd, true);
    }
    return fd;
}

bool SocketUtil::SetNonBlock(int fd, bool on)
{
    int flags = ::fcntl(fd, F_GETFL);
    if(on)
    {
        flags |= O_NONBLOCK;
    }
    else
    {
        flags &= (~O_NONBLOCK);
    }
    return ::fcntl(fd, F_SETFL, flags) == 0;
}

bool SocketUtil::IsNonBlock(int fd)
{
    int flags = ::fcntl(fd, F_GETFL);
    return (flags & O_NONBLOCK);
}


bool SocketUtil::SetReuseAddr(int fd, bool on)
{
    int option = on ? 1 : 0;
    if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char*)&option, sizeof(option)) != 0)
    {
        return false;
    }
    #if defined(REUSEPORT)
    if(setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, (char*)&option, sizeof(option)) != 0)
    {
        return false;
    }
    #endif
    return true;
}

int SocketUtil::GetError(int fd)
{
    int optval;
    socklen_t optlen = sizeof(optval);

    if (::getsockopt(fd, SOL_SOCKET, SO_ERROR, &optval, &optlen) == 0)
    {
        return optval;
    }
    return errno;

}

#if 0
bool SocketUtil::SetSendTimeout(uint32_t timeout_ms)
{
    struct timeval tv;
    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;        
    return (setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (const char*)&tv, sizeof(tv)) == 0);

}
bool SocketUtil::SetRecvTimeout(uint32_t timeout_ms)
{
    struct timeval tv;
    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;        
    return (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv)) == 0);
}
#endif

int SocketUtil::Select(int fd, int events, uint32_t timeout_ms)
{
    int ret;
    for(;;)
    {
        struct pollfd fds[1];
        fds[0].fd = fd;
        fds[0].events = events;      
        fds[0].revents = 0;

        ret = poll(fds, 1, timeout_ms);
        if(ret > 0)
        {
            if(fds[0].revents & (POLLERR|POLLHUP))
            {
                errno = GetError(fd);
                return -1;
            }
            else
            {
                assert(fds[0].revents > 0);
                return fds[0].revents;
            }
        }
        else if(ret == 0)
        {
            errno = ERR_TIMEOUT;
            break;
        }
        else if(errno != EINTR)
        {
            break;
        }
    }
    return ret;
}

}
