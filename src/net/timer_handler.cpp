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
#include "timer_handler.h"

using namespace xfutil;

namespace xfnet
{

bool TimerHandler::HandleRead()
{
    uint64_t exp;

    ssize_t rsize = read(m_timer.fd(), &exp, sizeof(exp));
    if (rsize == sizeof(uint64_t))
    {
        TimerHandlerPtr handler = std::dynamic_pointer_cast<TimerHandler>(shared_from_this());
        m_handler_queue.Push(handler);
    }
    return true;
}


} 


