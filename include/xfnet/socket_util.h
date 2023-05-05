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

#ifndef __xfnet_socket_util_h__
#define __xfnet_socket_util_h__

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include "xfnet.h"

using namespace xfutil;

namespace xfnet 
{

class SocketUtil
{
public:
    static int Open(int family = AF_INET);
    static bool SetReuseAddr(int fd, bool on);
    static bool SetNonBlock(int fd, bool on); 
    static bool IsNonBlock(int fd);
    static bool SetNoDelay(int fd, bool on)
    {
        int option = on ? 1 : 0;
        return setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char*)&option, sizeof(option));
    }

    static bool SetKeepAlive(int fd, bool on)
    {
        int optval = on ? 1 : 0;
        return (setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (const char*)&optval, sizeof(optval)) == 0);

    }
    static int GetError(int fd);

    static const int EVENT_READ;
    static const int EVENT_WRITE;
    static const int EVENT_READWRITE;

    //-2表示对方socket关闭，-1表示错误，0表示超时，正数表示事件
    static int Select(int fd, int events, uint32_t timeout_ms);

};


} 

#endif

