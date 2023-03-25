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

#include <sys/epoll.h>
#include "xfnet.h"

using namespace xfutil;

#define MAX_EVENTS   1024

namespace xfnet
{

bool Selector::Open()
{
    assert(m_epoll_fd == INVALID_SOCKET);
    m_epoll_fd = epoll_create1(EPOLL_CLOEXEC);
    return (m_epoll_fd != INVALID_SOCKET);
}

void Selector::Close()
{
    if(m_epoll_fd != INVALID_SOCKET)
    {
        close(m_epoll_fd);
        m_epoll_fd = INVALID_SOCKET;
    }

}

bool Selector::Register(EventHandler* handler, uint32_t events)
{
    assert(handler->GetStream().fd() != INVALID_SOCKET);
    assert(m_epoll_fd != INVALID_SOCKET);        
    struct epoll_event ee;
    ee.events = events;
    ee.data.ptr = (void *)handler;

    assert(m_epoll_fd != INVALID_SOCKET);        
    return (epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, handler->GetStream().fd(), &ee) == 0);
}

bool Selector::Unregister(EventHandler* handler)
{
    assert(handler->GetStream().fd() != INVALID_SOCKET);
    assert(m_epoll_fd != INVALID_SOCKET);        
    struct epoll_event ee;
    ee.events = 0;
    ee.data.ptr = handler;
    return (epoll_ctl(m_epoll_fd, EPOLL_CTL_DEL, handler->GetStream().fd(), &ee) == 0);

}

bool Selector::Modify(EventHandler* handler, uint32_t events)
{
    assert(handler->GetStream().fd() != INVALID_SOCKET);
    struct epoll_event ee;

    ee.events = events;
    ee.data.ptr = (void *) handler;

    assert(m_epoll_fd != INVALID_SOCKET);        
    return (epoll_ctl(m_epoll_fd, EPOLL_CTL_MOD, handler->GetStream().fd(), &ee) == 0);

}

void Selector::Select(uint32_t timeout_ms)
{
    assert(m_epoll_fd != INVALID_SOCKET);        

    struct epoll_event event_array[MAX_EVENTS];
    int nfds = epoll_wait(m_epoll_fd, event_array, MAX_EVENTS, timeout_ms);   
    for(int i = 0; i < nfds; ++i)
    {
        EventHandler* handler = (EventHandler*)event_array[i].data.ptr;
        uint32_t events = event_array[i].events;

        if(events & (EPOLLIN|EPOLLPRI)) 
        {
            if(!handler->HandleRead())
            {
                continue;
            }
        }
        if(events & EPOLLOUT)
        {
            if(!handler->HandleWrite())
            {
                continue;
            }
        }
        if(events & (EPOLLERR|EPOLLHUP))
        {
            handler->HandleError();
        }        
    }
}



} 

