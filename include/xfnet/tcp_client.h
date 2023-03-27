/*************************************************************************
Copyright (C) 2022 The xfnet Authors. All rights reserved.

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

#ifndef __xfnet_tcp_client_h__
#define __xfnet_tcp_client_h__

#include "xfnet.h"
#include "xfnet/queue.h"

using namespace xfutil;

namespace xfnet
{

class TcpClient
{
public:
    TcpClient(CreateEventHandlerCallback cb, void* arg = nullptr);
    ~TcpClient()
    {}

public:
    bool Start(int work_thread_num);
    void Stop();

    void Connect(const Address& addr);
    bool Connect(const Address& addr, uint32_t timeout_ms);

private:
    bool Connect(uint32_t timeout_ms);
    static void ConnectThread(void* arg);

private:
    CreateEventHandlerCallback m_create_eventhandler;
    void* m_create_eventhandler_arg;

    volatile int m_state;

    Thread m_connect_thread;
    BlockingQueue<Address> m_connect_queue;
    BlockingQueue<Address> m_failed_queue;

    uint32_t m_next_loop_index;
    uint32_t m_loop_num;
    EventLoopGroup m_loop_group;


private:

private:
	TcpClient(const TcpClient&) = delete;
	TcpClient& operator=(const TcpClient&) = delete;
};


} 

#endif

