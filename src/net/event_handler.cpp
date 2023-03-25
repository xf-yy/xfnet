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

namespace xfnet
{

const int EventHandler::EVENT_READ = EPOLLIN | EPOLLPRI;
const int EventHandler::EVENT_WRITE = EPOLLOUT;
const int EventHandler::EVENT_READWRITE = EventHandler::EVENT_READ | EventHandler::EVENT_WRITE;

bool EventHandler::HandleRead()
{
    return true;
}

bool EventHandler::HandleWrite()
{
    return true;
}

void EventHandler::HandleError()
{
    EventHandlerPtr handler = shared_from_this();

    EventHandler* t = handler.get();
    assert(t->GetStream().fd() != INVALID_SOCKET);
    m_loop->Unregister(handler);
}





} 


