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

#ifndef __xfnet_timer_handler_h__
#define __xfnet_timer_handler_h__

#include "xfnet.h"
#include "timer.h"

using namespace xfutil;

namespace xfnet
{

class EventLoop;

class TimerHandler : public EventHandler
{
public:
    TimerHandler(EventLoop* loop, Timer& timer, BlockingQueue<TimerHandlerPtr>& handler_queue)
    : EventHandler(loop), m_timer(std::move(timer)), m_handler_queue(handler_queue)
    {}

    virtual fd_t fd() const
    {
        return m_timer.fd();
    }
    void HandleTimeout()
    {
        m_timer.Execute();
    }
protected:
    //继续处理时返回true，否则返回false
    virtual bool HandleRead();
    
protected:
    Timer m_timer;
    BlockingQueue<TimerHandlerPtr>& m_handler_queue;

private:
	TimerHandler(const TimerHandler&) = delete;
	TimerHandler& operator=(const TimerHandler&) = delete;
};



} 

#endif

