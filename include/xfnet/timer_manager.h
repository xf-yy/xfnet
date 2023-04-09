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

#ifndef __xfnet_timer_manager_h__
#define __xfnet_timer_manager_h__

#include "xfnet.h"
#include "xfnet/queue.h"

using namespace xfutil;

namespace xfnet
{

typedef void (*TimerHandlerCallback)(void* timer, void* arg);
class TimerManager
{
public:
    TimerManager();
    ~TimerManager()
    {}

public:
    bool Start(int work_thread_num);
    void Stop();

    void* AddTimer(uint32_t interval_ms, TimerHandlerCallback cb, void* arg = nullptr);
    bool SetTimer(void* timer, uint32_t interval_ms);
    bool RemoveTimer(void* timer);
    
private:
    static void WorkThread(int index, void* arg);

private:
    BlockingQueue<TimerHandlerPtr> m_handler_queue;
    ThreadGroup m_work_threads;

    EventLoop m_loop;

private:
	TimerManager(const TimerManager&) = delete;
	TimerManager& operator=(const TimerManager&) = delete;
};


} 

#endif

