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

#ifndef __xfnet_address_h__
#define __xfnet_address_h__

#include "xfnet.h"
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace xfutil;

namespace xfnet
{

class Address
{
public:
    Address()
    {
        memset(&m_addr6, 0x00, sizeof(m_addr6));
    }
    Address(const char* ip, uint16_t port);
    Address(uint16_t port, int family = AF_INET);

    enum
    {
        MAX_IP_LEN = 48,
        MAX_ADDR_LEN = 56,
    };
    
public:
    struct sockaddr* SockAddr() const
    {
        return (struct sockaddr*)&m_addr6;
    }
    int Size() const
    {
        return sizeof(m_addr6);
    }

    int Family() const
    {
        return m_addr4.sin_family;
    }
    bool GetIP(char ip[MAX_IP_LEN]) const;
    uint16_t Port() const
    {
        return ntohs(m_addr4.sin_port); 
    }

    bool ToString(char addr[MAX_ADDR_LEN]) const;

private:
    union
    {
        struct sockaddr_in m_addr4;
        struct sockaddr_in6 m_addr6;
    };
};


} 

#endif

