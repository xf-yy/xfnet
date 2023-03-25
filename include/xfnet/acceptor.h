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

#ifndef __xfnet_acceptor_h__
#define __xfnet_acceptor_h__

#include <netinet/tcp.h>
#include "xfnet.h"

using namespace xfutil;

namespace xfnet
{

class Acceptor 
{
public:
    Acceptor()
    {
        m_fd = INVALID_SOCKET;
    }
    ~Acceptor()
    {
        Close();
    }
public:
    bool Open(const Address& addr, int backlog, int family = AF_INET);

    void Close();

    Stream Accept(Address& addr, uint32_t timeout_ms);

private:
    int m_fd;

private:
	Acceptor(const Acceptor&) = delete;
	Acceptor& operator=(const Acceptor&) = delete;
};


} 

#endif

