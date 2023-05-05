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
#include "xfnet.h"

using namespace xfutil;

namespace xfnet 
{

Stream::Stream(int fd/* = INVALID_SOCKET*/)
{
    m_fd = fd;
    if(fd != INVALID_SOCKET)
    {
        assert(SocketUtil::IsNonBlock(fd));
        SocketUtil::SetNoDelay(fd, true);
        SocketUtil::SetKeepAlive(fd, true);
    }
}

Stream::~Stream()
{
    if(m_fd != INVALID_SOCKET)
    {
        close(m_fd);
        m_fd = INVALID_SOCKET;
    }
}

#if 0
bool Stream::SetSendTimeout(uint32_t timeout_ms)
{
    struct timeval tv;
    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;        
    return (setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (const char*)&tv, sizeof(tv)) == 0);

}
bool Stream::SetRecvTimeout(uint32_t timeout_ms)
{
    struct timeval tv;
    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;        
    return (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv)) == 0);
}
#endif

//-1:表示发送错误，0表示部分发送成功，1表示全部发送成功
int Stream::Send(void*& data, ssize_t& size)
{
    while(size > 0)
    {
        int wsize = write(m_fd, data, size);
        if(wsize >= 0)
        {
            size -= wsize;
            data = (char*)data + size;
        }
        else
        { 
            if(errno == EAGAIN)
            {
                return 0;
            }
            else if(errno != EINTR)
            {
                return -1;
            }
            else
            {
                continue;
            }
        }
    }
    return 1;
}    

static void UpdateIov(iovec_t*& iov, int& iovcnt, ssize_t size)
{
    assert(size >= 0);
    for(;;)
    {
        if(size >= (ssize_t)iov->iov_len)
        {
            size -= iov->iov_len;
            ++iov;
            --iovcnt;
        }
        else
        {
            iov->iov_base = (char*)iov->iov_base + size;
            iov->iov_len -= size;
            break;
        }
    }
}

int Stream::Send(iovec_t*& iov, int& iovcnt)
{
    while(iovcnt > 0)
    {
        ssize_t wsize = writev(m_fd, iov, iovcnt);
        if(wsize >= 0)
        {
            UpdateIov(iov, iovcnt, wsize);
        }
        else
        { 
            if(errno == EAGAIN)
            {
                return 0;
            }
            else if(errno != EINTR)
            {
                return -1;
            }
            else
            {
                continue;
            }
        }
    }
    return 1;
}

int Stream::Send(const void* data, ssize_t size, uint32_t timeout_ms)
{
    while(size > 0)
    {
        ssize_t wsize = write(m_fd, data, size);
        if(wsize >= 0)
        {
            data = (char*)data + wsize;
            size -= wsize;
        }
        else
        {
            if(errno == EAGAIN)
            {
                int events = SocketUtil::Select(m_fd, SocketUtil::EVENT_WRITE, timeout_ms);
                if(events <= 0)
                {
                    return events;
                }
            }
            else if(errno != EINTR)
            {
                return -1;
            }
        }
    }
    return 1;
}

//-2：表示对方socket关闭，-1:表示介绍错误，0表示部分接收成功，1表示全部接收成功
int Stream::Recv(void*& data, ssize_t& size)
{
    while(size > 0)
    {
        int rsize = read(m_fd, data, size);
        if(rsize > 0)
        {
            size -= rsize;
            data = (char*)data + size;
        }
        else if(rsize == 0)
        {
            return -2;
        }
        else
        {
            if(errno == EAGAIN)
            {
                return 0;
            }
            else if(errno != EINTR)
            {
                return -1;
            }
            else
            {
                continue;
            }
        }
    }
    return 1;
}

int Stream::Recv(iovec_t*& iov, int& iovcnt)
{
    while(iovcnt > 0)
    {
        int rsize = readv(m_fd, iov, iovcnt);
        if(rsize > 0)
        {
            UpdateIov(iov, iovcnt, rsize);
        }
        else if(rsize == 0)
        {
            return -2;
        }
        else
        {
            if(errno == EAGAIN)
            {
                return 0;
            }
            else if(errno != EINTR)
            {
                return -1;
            }
            else
            {
                continue;
            }
        }
    }
    return 1;
}

int Stream::Recv(void *buf, size_t size, uint32_t timeout_ms)
{
    while(size > 0)
    {
        ssize_t rsize = read(m_fd, buf, size);
        if(rsize > 0)
        {
            buf = (char*)buf + rsize;
            size -= rsize;
        }
        else if(rsize == 0)
        {
            assert(rsize == 0);
            return -2;
        }
        else
        {
            if(errno == EAGAIN)
            {
                int events = SocketUtil::Select(m_fd, SocketUtil::EVENT_READ, timeout_ms);
                if(events <= 0)
                {
                    return events;
                }
            }
            else if(errno != EINTR)
            {
                return -1;
            }
        }
    }
    return 1;
} 

}
