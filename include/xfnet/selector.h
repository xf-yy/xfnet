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

#ifndef __xfnet_selector_h__
#define __xfnet_selector_h__

#include "xfnet.h"

using namespace xfutil;

#define MAX_EVENTS   1024

namespace xfnet
{

class Selector
{
public:
    Selector()
    {
        m_epoll_fd = INVALID_SOCKET;
    }
    ~Selector()
    {
        Close();
    }

public:
    bool Open();
    void Close();

    bool Register(EventHandler* handler, uint32_t events);
    bool Unregister(EventHandler* EventHandler);
    bool Modify(EventHandler* handler, uint32_t events);

    void Select(uint32_t timeout_ms);

private:
    int m_epoll_fd;

private:
	Selector(const Selector&) = delete;
	Selector& operator=(const Selector&) = delete;
};


} 

#endif

