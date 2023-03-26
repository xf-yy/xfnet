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


#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include "xfnet.h"

using namespace xfutil;

namespace xfnet 
{

Stream Connector::Connect(Address& addr, int timeout_ms)
{
    int fd = SocketUtil::Open(addr.Family());
    if(fd == INVALID_SOCKET)
    {
        return Stream(fd);
    }    
    for(;;)
    {
        int ret = connect(fd, addr.SockAddr(), addr.Size());
        if(ret == 0)
        {
            return Stream(fd);
        }
        if(errno == EINPROGRESS)
        {
            int events = SocketUtil::Select(fd, SocketUtil::EVENT_WRITE, timeout_ms);
            if(events <= 0)
            {
                break;
            }
        }
        else if(errno != EINTR)
        {
            break;
        }
    }
    close(fd);
    fd = INVALID_SOCKET;
    return Stream(fd);        

}


} 


