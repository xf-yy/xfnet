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
        m_state = STATE_STARTED;
        m_selector.Open();
        m_thread.Start(LoopThread, this);
        return true;
    }

    void EventLoop::Stop()
    {
        if(m_state != STATE_STARTED)
        {
            return;
        }
        m_state = STATE_STOPPING;
        m_thread.Join();
        m_selector.Close();
        m_state = STATE_STOPPED;
    }

    bool EventLoop::Register(EventHandlerPtr& handler, uint32_t events)
    {
        m_mutex.lock();
        m_handlers[handler.get()] = handler;
        m_mutex.unlock();

        return m_selector.Register(handler.get(), events);
    }
    
    bool EventLoop::Unregister(EventHandlerPtr& handler)
    {
        EventHandler* h = handler.get();

        m_mutex.lock();
        size_t cnt = m_handlers.erase(h);
        m_mutex.unlock();     
        
        return cnt == 0 || m_selector.Unregister(h);
    }


    void EventLoop::LoopThread(void* arg)
    {
        EventLoop* loop = (EventLoop*)arg;
        assert(loop != nullptr);

        while(loop->m_state == STATE_STARTED)
        {
            loop->m_selector.Select(10*1000);

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
            m_loops[i]->Stop();
        }
    }

} 


