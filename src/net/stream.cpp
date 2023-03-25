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
            SocketUtil::SetNonBlock(fd, true);
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
    int Stream::Send(const void* data, ssize_t size, ssize_t& sent_size)
    {
        while(sent_size < size)
        {
            int wsize = write(m_fd, (char*)data+sent_size, size-sent_size);
            if(wsize == -1)
            {
                if(errno == EINTR)
                {
                    continue;
                }
                else if(errno == EAGAIN)
                {
                    return 0;
                }
                else
                {
                    return -1;
                }
            }
            sent_size += wsize;
        }
        return 1;
    }    
    ssize_t Stream::Send(const void* data, ssize_t size, uint32_t timeout_ms)
    {
        while(size > 0)
        {
            ssize_t wsize = write(m_fd, data, size);
            if(wsize == -1)
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
            else
            {
                data = (char*)data + wsize;
                size -= wsize;
            }

        }
        return 1;
    }

    //-2：表示对方socket关闭，-1:表示介绍错误，0表示部分接收成功，1表示全部接收成功
    int Stream::Recv(void* data, ssize_t size, ssize_t& recv_size)
    {
        while(recv_size < size)
        {
            int rsize = read(m_fd, (char*)data+recv_size, size-recv_size);
            if(rsize == -1)
            {
                if(errno == EINTR)
                {
                    continue;
                }
                else if(errno == EAGAIN)
                {
                    return 0;
                }
                else
                {
                    return -1;
                }
            }
            else if(rsize == 0)
            {
                return -2;

            }
            recv_size += rsize;
        }
        return 1;
    }
    ssize_t Stream::Recv(void *buf, size_t size, uint32_t timeout_ms)
    {
        while(size > 0)
        {
            ssize_t rsize = read(m_fd, buf, size);
            if(rsize == -1)
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
            else if(rsize > 0)
            {
                buf = (char*)buf + rsize;
                size -= rsize;
            }
            else
            {
                assert(rsize == 0);
                return -1;
            }
        }
        return 1;
    } 

}
