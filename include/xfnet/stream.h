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

#ifndef __xfnet_stream_h__
#define __xfnet_stream_h__

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include "xfnet.h"

using namespace xfutil;

namespace xfnet 
{

#ifdef _WIN32
struct iovec_t
{
    void  *iov_base;   
    size_t iov_len;  
};
#else
typedef struct iovec iovec_t;
#endif

//非阻塞模式
class Stream
{
public:
    explicit Stream(int fd = INVALID_SOCKET);

    // 移动构造函数
    Stream(Stream&& other)
    {
        m_fd = other.m_fd;
        other.m_fd = INVALID_SOCKET;
    }
   
    ~Stream();

public:
    bool Valid() const
    {
        return m_fd != INVALID_SOCKET;
    }
    int fd() const
    {
        return m_fd;
    }
    bool Shutdown()
    {
        return (::shutdown(m_fd, SHUT_RDWR) == 0);
    }

    bool SetRecvBufferSize(int size)
    {
        return (setsockopt(m_fd, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size)) == 0);

    }
    bool SetSendBufferSize(int size)
    {
        return (setsockopt(m_fd, SOL_SOCKET, SO_SNDBUF, &size, sizeof(size)) == 0);
    }

    bool GetLocalAddress(Address& addr)
    {
        socklen_t size = addr.Size();
        return (getsockname(m_fd, addr.SockAddr(), &size) == 0);
    }
    bool GetRemoteAddress(Address& addr)
    {
        socklen_t size = addr.Size();
        return (getpeername(m_fd, addr.SockAddr(), &size) == 0);
    }

    //-1:表示发送错误，0表示部分发送成功，1表示全部发送成功
    int Send(void*& data, ssize_t& size);
    int Send(iovec_t*& iov, int& iovcnt);
    //-1:表示发送错误，0表示超时，1表示全部发送成功
    int Send(const void* data, ssize_t size, uint32_t timeout_ms);
    
    //-2：表示对方socket关闭，-1:表示接收错误，0表示部分接收成功，1表示全部接收成功
    int Recv(void*& data, ssize_t& size);
    int Recv(iovec_t*& iov, int& iovcnt);
    //-2：表示对方socket关闭，-1:表示接收错误，0表示超时，1表示全部接收成功    
    int Recv(void *buf, size_t size, uint32_t timeout_ms);  

private:
    int m_fd;

private:
	Stream(const Stream&) = delete;
	Stream& operator=(const Stream&) = delete;
};


} 

#endif

