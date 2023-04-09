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

#include "xfnet.h"
#include "common.h"

using namespace xfutil;

namespace xfnet
{

TcpClient::TcpClient(CreateStreamHandlerCallback cb, void* arg/* = nullptr*/) 
    : m_create_eventhandler(cb), m_create_eventhandler_arg(arg)
{
    m_loop_num = 0;
    m_next_loop_index = 0;
    m_state = STATE_STOPPED;
}

bool TcpClient::Start(int work_thread_num)
{
    assert(m_state == STATE_STOPPED);

    m_loop_num = work_thread_num;
    if(!m_loop_group.Start(work_thread_num))
    {
        return false;
    }
    if(!m_connect_thread.Start(ConnectThread, this))
    {
        return false;
    }
    m_state = STATE_STARTED;
    return true;
}

void TcpClient::Stop()
{
    if(m_state != STATE_STARTED)
    {
        return;
    }
    m_state = STATE_STOPPING;
    m_connect_thread.Join();
    m_loop_group.Stop();

    m_state = STATE_STOPPED;

}

//为一次性连接，如果连接断了，需要重新加入
void TcpClient::Connect(const Address& addr)
{
    m_connect_queue.Push(addr);
}

bool TcpClient::Connect(const Address& addr, uint32_t timeout_ms)
{
    Stream stream = Connector::Connect(addr, timeout_ms);
    if(!stream.Valid())
    {
        return false;
    }

    assert(m_loop_num != 0);
    EventLoop* loop = m_loop_group.GetEventLoop(m_next_loop_index++ % m_loop_num);

    return m_create_eventhandler(loop, stream, m_create_eventhandler_arg); 
}

bool TcpClient::Connect(uint32_t timeout_ms)
{
    Address addr;
    if(!m_connect_queue.Pop(addr, timeout_ms))
    {
        //将failed中的addr迁移至connect队列
        while(m_failed_queue.TryPop(addr))
        {
            m_connect_queue.Push(addr);
        }
        return false;
    }

    if(Connect(addr, timeout_ms))
    {
        return true;
    }
    m_failed_queue.Push(addr);
    return false;
}

void TcpClient::ConnectThread(void* arg)
{
    TcpClient* client = (TcpClient*)arg; 
    while(client->m_state != STATE_STOPPING)
    {
        client->Connect(10*1000);
    }
}




} 


