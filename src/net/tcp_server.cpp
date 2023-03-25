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
#include "common.h"

using namespace xfutil;

namespace xfnet
{

TcpServer::TcpServer(CreateEventHandlerCallback cb, void* arg/* = nullptr*/) 
    : m_create_eventhandler(cb), m_create_eventhandler_arg(arg)
{
    m_loop_num = 0;
    m_next_loop_index = 0;
    m_state = STATE_STOPPED;
}

TcpServer::~TcpServer()
{
    assert(m_state == STATE_STOPPED);
}

bool TcpServer::Start(const Address& addr, int backlog, int work_thread_num/* = 4*/)
{
    assert(m_state == STATE_STOPPED);

    m_state = STATE_STARTED;
    m_acceptor.Open(addr, backlog);

    m_loop_num = work_thread_num;
    m_loop_group.Start(work_thread_num);
    return m_accept_thread.Start(AcceptThread, this);
}

void TcpServer::Stop()
{
    if(m_state != STATE_STARTED)
    {
        return;
    }
    m_accept_thread.Join();
    m_loop_group.Stop();
    m_acceptor.Close();
    m_state = STATE_STOPPED;
}


bool TcpServer::Accept(uint32_t timeout_ms)
{    
    Address addr;
    Stream stream = m_acceptor.Accept(addr, timeout_ms);
    if(!stream.Valid())
    {
        //输出日志
        return false;
    }
    assert(m_loop_num != 0);
    EventLoop* loop = m_loop_group.GetEventLoop(m_next_loop_index++ % m_loop_num);

    return m_create_eventhandler(loop, stream, m_create_eventhandler_arg);
}

void TcpServer::AcceptThread(void* arg)
{
    TcpServer* server = (TcpServer*)arg;
    while(server->m_state == STATE_STARTED)
    {
        server->Accept(10*1000);
    }
}


} 


