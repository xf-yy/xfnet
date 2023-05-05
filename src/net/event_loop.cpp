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

#include "xfnet.h"
#include "common.h"

using namespace xfutil;

namespace xfnet
{
    #define SELECT_TIMEOUT  5000

    EventLoop::EventLoop()
    {
        m_state = STATE_STOPPED;
    }

    EventLoop::~EventLoop()
    {
        assert(m_state == STATE_STOPPED);
    }    
    
    bool EventLoop::Start()
    {
        m_selector.Open();
        if(!m_thread.Start(LoopThread, this))
        {
            return false;
        }
        m_state = STATE_STARTED;
        return true;
    }

    bool EventLoop::Stopping()
    {
        if(m_state == STATE_STARTED)
        {
            m_state = STATE_STOPPING;
            return true;
        }
        return false;
    }

    void EventLoop::Join()
    {
        m_thread.Join();
        m_selector.Close();
        m_state = STATE_STOPPED;
    }

    void EventLoop::Stop()
    {
        if(Stopping())
        {
            Join();
        }
    }

    bool EventLoop::Register(EventHandlerPtr& handler, uint32_t events)
    {
        m_mutex.lock();
        auto pr = m_handlers.insert(std::make_pair(handler.get(), handler));
        m_mutex.unlock();

        if(!pr.second)
        {
            return false;
        }
        return m_selector.Register(handler.get(), events);
    }
    
    bool EventLoop::Unregister(EventHandler* handler)
    {
        EventHandlerPtr h_ptr;

        m_mutex.lock();
        auto it = m_handlers.find(handler);
        if(it != m_handlers.end())
        {
            h_ptr = it->second;
            m_handlers.erase(it);
        }
        m_mutex.unlock();     
        
        return (!h_ptr) || m_selector.Unregister(handler);
    }


    void EventLoop::LoopThread(void* arg)
    {
        EventLoop* loop = (EventLoop*)arg;
        assert(loop != nullptr);

        while(loop->m_state != STATE_STOPPING)
        {
            loop->m_selector.Select(SELECT_TIMEOUT);
        }
    }    
    
    bool EventLoopGroup::Start(int loop_count)
    {
        std::vector<EventLoopPtr> loops;
        loops.resize(loop_count);

        for(int i = 0; i < loop_count; ++i)
        {
            loops[i] = NewEventLoop();
            loops[i]->Start();
        }
        m_loops.swap(loops);
        return true;
    }

    void EventLoopGroup::Stop()
    {
        for(size_t i = 0; i < m_loops.size(); ++i)
        {
            m_loops[i]->Stopping();
        }
        for(size_t i = 0; i < m_loops.size(); ++i)
        {
            m_loops[i]->Join();
        }        
    }

} 


