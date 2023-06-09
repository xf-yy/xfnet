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
#include "xfnet.h"

using namespace xfutil;

namespace xfnet
{

bool Acceptor::Open(const Address& addr, int backlog, int family/* = AF_INET*/)
{
    m_fd = SocketUtil::Open(family);
    if(m_fd == INVALID_SOCKET)
    {
        return false;
    }
    SocketUtil::SetReuseAddr(m_fd, true);
    if(bind(m_fd, addr.SockAddr(), addr.Size()) != 0)
    {
        Close();
        return false;
    }
    listen(m_fd, backlog);

    return true;    
}

void Acceptor::Close()
{
    if(m_fd != INVALID_SOCKET)
    {
        close(m_fd);
        m_fd = INVALID_SOCKET;
    }
}

Stream Acceptor::Accept(Address& addr)
{
    socklen_t len = addr.Size();
    int fd = accept4(m_fd, addr.SockAddr(), &len, SOCK_NONBLOCK | SOCK_CLOEXEC); 
    return Stream(fd);    
}

Stream Acceptor::Accept(Address& addr, uint32_t timeout_ms)
{
    socklen_t len = addr.Size();
    int fd = accept4(m_fd, addr.SockAddr(), &len, SOCK_NONBLOCK | SOCK_CLOEXEC); 
    if(fd == INVALID_SOCKET)
    {
        int events = SocketUtil::Select(m_fd, SocketUtil::EVENT_READ, timeout_ms);
        if(events > 0 && (events & SocketUtil::EVENT_READ))
        {
            fd = accept4(m_fd, addr.SockAddr(), &len, SOCK_NONBLOCK | SOCK_CLOEXEC); 
        }
    }

    return Stream(fd);

}







} 


