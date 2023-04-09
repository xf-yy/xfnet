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

#ifndef __xfnet_event_loop_h__
#define __xfnet_event_loop_h__

#include <unordered_map>
#include <atomic>
#include "xfnet.h"

using namespace xfutil;

namespace xfnet
{

class EventLoop
{
public:
    EventLoop();
    ~EventLoop();

    bool Start();
    void Stop();

    bool Register(EventHandlerPtr& handler, uint32_t events);
    bool Unregister(EventHandler* handler);
    bool Modify(EventHandler* handler, uint32_t events)
    {
        return m_selector.Modify(handler, events);
    }    

private:    
    bool Stopping();
    void Join();

    static void LoopThread(void* arg);

private:    
    volatile int m_state;

    Selector m_selector;
    Thread m_thread;

    std::mutex m_mutex;
    std::unordered_map<EventHandler*, EventHandlerPtr> m_handlers;

    friend class EventLoopGroup;
private:
	EventLoop(const EventLoop&) = delete;
	EventLoop& operator=(const EventLoop&) = delete;

};
typedef std::shared_ptr<EventLoop> EventLoopPtr;
#define NewEventLoop std::make_shared<EventLoop>

class EventLoopGroup
{
public:
    EventLoopGroup() 
    {
    }
    ~EventLoopGroup()
    {
    }
    
public:
    bool Start(int loop_count);
    void Stop();

    size_t Size()
    {
        return m_loops.size();
    }
    EventLoop* GetEventLoop(uint32_t index)
    {
        assert(index < m_loops.size());
	    return m_loops[index].get();
    }
    
private:
    std::vector<EventLoopPtr> m_loops;

private:
	EventLoopGroup(const EventLoopGroup&) = delete;
	EventLoopGroup& operator=(const EventLoopGroup&) = delete;
};


} 

#endif

