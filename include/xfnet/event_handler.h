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

#ifndef __xfnet_event_handler_h__
#define __xfnet_event_handler_h__

#include "xfnet.h"

using namespace xfutil;

namespace xfnet
{

class EventLoop;

class EventHandler : public std::enable_shared_from_this<EventHandler>
{
public:
    EventHandler(EventLoop* loop, Stream& stream)
    : m_loop(loop), m_stream(std::move(stream))
    {}

    virtual ~EventHandler()
    {
    }

    EventLoop* GetEventLoop()
    {
        return m_loop;
    }
    Stream& GetStream()
    {
        return m_stream;
    }

    static const int EVENT_READ;
    static const int EVENT_WRITE;
    static const int EVENT_READWRITE;

protected:
    //继续处理时返回true，否则返回false
    virtual bool HandleRead();
    virtual bool HandleWrite();
    virtual void HandleError();
    
protected:
    EventLoop* m_loop;
    Stream m_stream;

    friend class Selector;
private:
	EventHandler(const EventHandler&) = delete;
	EventHandler& operator=(const EventHandler&) = delete;
};

typedef std::shared_ptr<EventHandler> EventHandlerPtr;
#define NewEventHandler std::make_shared<EventHandler>

} 

#endif

