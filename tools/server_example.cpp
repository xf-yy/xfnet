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

#include <iostream>
#include <vector>
#include <string>
#include <deque>
#include "xfnet.h"

using namespace xfutil;
using namespace xfnet;

#define PORT    1981

//通讯协议[body_size][body]
//        4B         size

class TestEventHandler : public EventHandler
{
public:
    TestEventHandler(EventLoop* loop, Stream& stream) : EventHandler(loop, stream)
    {
        m_recv_head_size = 0;
        m_recv_pkt_size = 0;

        m_send_off = 0;
    }
public:
    bool Send(std::string& data)
    {
		std::lock_guard<std::mutex> lock(m_mutex);
        m_send_datas.push_back(data);
        m_loop->Modify(this, EventHandler::EVENT_READWRITE);
        return true;
    }
    virtual bool HandleRead() override
    {
        for(;;)
        {
            if(m_recv_head_size < HEAD_SIZE)
            {
                int ret = m_stream.Recv(m_head, HEAD_SIZE, m_recv_head_size);
                if(ret < 0)
                {
                    EventHandlerPtr handler = shared_from_this();
                    m_loop->Unregister(handler);
                    return false;
                }
                else if(ret == 0)
                {
                    break;
                }
                assert(m_recv_head_size == HEAD_SIZE);

                m_pkt_size = *(int*)m_head + HEAD_SIZE;
                m_pkt_data.resize(m_pkt_size);
                *(int*)&m_pkt_data[0] = m_pkt_size - HEAD_SIZE;
                m_recv_pkt_size = HEAD_SIZE;

            }
            else
            {
                int ret = m_stream.Recv(&m_pkt_data[0], m_pkt_size, m_recv_pkt_size);
                if(ret < 0)
                {
                    EventHandlerPtr handler = shared_from_this();
                    m_loop->Unregister(handler);
                    return false;
                }
                else if(ret == 0)
                {
                    break;
                }
                assert(m_recv_pkt_size == m_pkt_size);

                Send(m_pkt_data);
                m_recv_head_size = 0;
                m_recv_pkt_size = 0;

            }
        }
        return true;
    }    
    virtual bool HandleWrite() override
    {
        for(;;)
        {            
            if((size_t)m_send_off == m_send_data.size())
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                if(m_send_datas.empty())
                {
                    m_loop->Modify(this, EventHandler::EVENT_READ);
                    break;
                }
                m_send_data = m_send_datas.front();
                m_send_datas.pop_front();
                m_send_off = 0;
            }
            assert((size_t)m_send_off != m_send_data.size());
            int ret = m_stream.Send(m_send_data.data(), m_send_data.size(), m_send_off);
            if(ret < 0)
            {
                EventHandlerPtr handler = shared_from_this();
                m_loop->Unregister(handler);
                return false;
            }
            else if(ret == 0)
            {
                break;
            }
            assert((size_t)m_send_off == m_send_data.size());

        }
        return true;
    }

    static bool CreateEventHandler(EventLoop* loop, Stream& stream, void* arg)
    {
        std::shared_ptr<EventHandler> handler =  std::make_shared<TestEventHandler>(loop, stream);
        return loop->Register(handler, EventHandler::EVENT_READ);
    }

private:
    enum{HEAD_SIZE=4};

    std::mutex m_mutex;

    char m_head[HEAD_SIZE];
    std::string m_pkt_data;
    int m_pkt_size;
    ssize_t m_recv_head_size;
    ssize_t m_recv_pkt_size;

    std::deque<std::string> m_send_datas;
    std::string m_send_data;
    ssize_t m_send_off;
};


int main(int argc, char* argv[])
{
    Address addr("127.0.0.1", PORT);

    TcpServer server(TestEventHandler::CreateEventHandler, nullptr);
    bool ret = server.Start(addr, 1024);
    if(!ret)
    {
        printf("cannot start server\n");
        return 1;
    }

    Thread::Sleep(100*1000);
	
    server.Stop();
	return 0;
}

