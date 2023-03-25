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


#include "xfnet.h"
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace xfutil;

namespace xfnet
{

Address::Address(const char* ip, uint16_t port)
{
    assert(ip != nullptr);
    bool ipv4 = (strchr(ip, ':') == NULL);

    if(ipv4)
    {
        memset(&m_addr4, 0x00, sizeof(m_addr4));
        
        m_addr4.sin_family = AF_INET;
        m_addr4.sin_port = htons(port);
        if(inet_pton(AF_INET, ip, &m_addr4.sin_addr) <= 0)
        {
            //
        }
    }
    else
    {
        memset(&m_addr6, 0x00, sizeof(m_addr6));

        m_addr6.sin6_family = AF_INET; 
        m_addr6.sin6_port = htons(port);            
        if(inet_pton(AF_INET6, ip, &m_addr6.sin6_addr) <= 0)
        {
            //
        }
    }
}

Address::Address(uint16_t port, int family/* = AF_INET*/)
{
    if(family == AF_INET)
    {
        m_addr4.sin_family = family; 
        m_addr4.sin_port = htons(port);            
        m_addr4.sin_addr.s_addr = htonl(INADDR_ANY);
    }
    else
    {
        m_addr6.sin6_family = family; 
        m_addr6.sin6_port = htons(port);            
        m_addr6.sin6_addr = in6addr_any;
    }
}

//ipv6端口表示：[ip]:port
//ipv4端口表示: ip:port
bool Address::ToString(char addr[MAX_ADDR_LEN]) const
{
    char ip[MAX_IP_LEN];
    GetIP(ip);

    if(m_addr4.sin_family == AF_INET)
    {
        snprintf(addr, MAX_ADDR_LEN, "%s:%u", ip, Port());

    }
    else
    {
       snprintf(addr, MAX_ADDR_LEN, "[%s]:%u", ip, Port());

    }
    return true;
}

bool Address::GetIP(char ip[MAX_IP_LEN]) const
{
    ip[0] = '\0';
    
    if(m_addr4.sin_family == AF_INET)
    {
        if(inet_ntop(AF_INET, &m_addr4.sin_addr, ip, sizeof(ip)) == NULL)
        {
            return false;
        }
    }
    else
    {
        if(inet_ntop(AF_INET6, &m_addr6.sin6_addr, ip, sizeof(ip)) == NULL)
        {
            return false;
        }
    }
    return true;
}



} 


