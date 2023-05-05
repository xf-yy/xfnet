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

#ifndef __xfnet_tcp_server_h__
#define __xfnet_tcp_server_h__

#include "xfnet.h"

using namespace xfutil;

namespace xfnet
{


class TcpServer
{
public:
    TcpServer(CreateStreamHandlerCallback cb, void* arg = nullptr);
    ~TcpServer();
    
public:
    //非线程安全
    bool Start(const Address& addr, int backlog, int work_thread_num = 4);
    void Stop();

private:
    bool Accept();
    static void AcceptThread(void* arg);

protected:
    CreateStreamHandlerCallback m_create_eventhandler_func;
    void* m_create_eventhandler_arg;

    volatile int m_state;
    
    Acceptor m_acceptor;
    Thread m_accept_thread;

    uint32_t m_next_loop_index;
    uint32_t m_loop_num;
    EventLoopGroup m_loop_group;

private:
	TcpServer(const TcpServer&) = delete;
	TcpServer& operator=(const TcpServer&) = delete;
};


} 

#endif

