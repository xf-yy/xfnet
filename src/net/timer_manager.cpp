/*************************************************************************
Copyright (C) 2022 The xfnet Authors. All rights reserved.

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
#include "timer_handler.h"

using namespace xfutil;

namespace xfnet
{

TimerManager::TimerManager() 
{
}

bool TimerManager::Start(int work_thread_num)
{
    assert(m_work_threads.Size() == 0);

    if(!m_loop.Start())
    {
        return false;
    }
    m_work_threads.Start(work_thread_num, WorkThread, this);
    return true;
}

void TimerManager::Stop()
{
    TimerHandlerPtr handler;
    for(size_t i = 0; i < m_work_threads.Size(); ++i)
    {
        m_handler_queue.Push(handler);
    }
    m_work_threads.Join();

    m_loop.Stop();
}

static void InitTime(struct itimerspec& itime, uint32_t interval_ms)
{
    itime.it_value.tv_sec = (int) interval_ms/1000;
    itime.it_value.tv_nsec = (interval_ms % 1000) * 1000000ULL;
    itime.it_interval.tv_sec = itime.it_value.tv_sec;
    itime.it_interval.tv_nsec = itime.it_value.tv_nsec;
}

void* TimerManager::AddTimer(uint32_t interval_ms, TimerHandlerCallback cb, void* arg/* = nullptr*/)
{
    Timer timer(cb, arg);
    if(timer.fd() == INVALID_SOCKET)
    {
        return nullptr;
    }
    struct itimerspec itime;
    InitTime(itime, interval_ms);
    if(timerfd_settime(timer.fd(), 0, &itime, NULL) != 0)
    {
        return nullptr;
    }

    EventHandlerPtr handler = NewTimerHandler(&m_loop, timer, m_handler_queue);
    if(!m_loop.Register(handler, EventHandler::EVENT_READ))
    {
        return nullptr;
    }

    return handler.get();
}

bool TimerManager::SetTimer(void* timer, uint32_t interval_ms)
{
    TimerHandler* handler = (TimerHandler*)timer;

    struct itimerspec itime;
    InitTime(itime, interval_ms);
    return (timerfd_settime(handler->fd(), 0, &itime, NULL) == 0);
}

bool TimerManager::RemoveTimer(void* timer)
{
    TimerHandler* handler = (TimerHandler*)timer;

    return m_loop.Unregister(handler);
}


void TimerManager::WorkThread(int index, void* arg)
{
    TimerManager* mgr = (TimerManager*)arg; 
    for(;;)
    {
        TimerHandlerPtr handler;
        mgr->m_handler_queue.Pop(handler);

        if(!handler)
        {
            break;
        }
        handler->HandleTimeout();
    }
}




} 


