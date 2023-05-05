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

class TestEventHandler : public StreamHandler
{
public:
    TestEventHandler(EventLoop* loop, Stream& stream) : StreamHandler(loop, stream)
    {
        m_head_ptr = m_head;
        m_head_size = HEAD_SIZE;

        m_body_size = 0;
        m_send_size = 0;
    }
public:
    bool Send(std::string& data)
    {
		std::lock_guard<std::mutex> lock(m_mutex);
        m_send_datas.push_back(data);
        m_loop->Modify(this, EVENT_READWRITE);
        return true;
    }
    virtual bool HandleRead() override
    {
        for(;;)
        {
            if(m_head_size != 0)
            {
                int ret = m_stream.Recv(m_head_ptr, m_head_size);
                if(ret < 0)
                {
                    m_loop->Unregister(this);
                    return false;
                }
                else if(ret == 0)
                {
                    break;
                }
                assert(m_head_size == 0);

                m_body_size = *(int*)m_head;
                m_pkt_data.resize(HEAD_SIZE + m_body_size);
                *(int*)&m_pkt_data[0] = m_body_size;
                m_body_ptr = (char*)m_pkt_data.data() + HEAD_SIZE;
            }
            else
            {
                int ret = m_stream.Recv(m_body_ptr, m_body_size);
                if(ret < 0)
                {
                    m_loop->Unregister(this);
                    return false;
                }
                else if(ret == 0)
                {
                    break;
                }
                assert(m_body_size == 0);

                Send(m_pkt_data);
                m_head_ptr = m_head;
                m_head_size = HEAD_SIZE;
            }
        }
        return true;
    }    
    virtual bool HandleWrite() override
    {
        for(;;)
        {            
            if(m_send_size == 0)
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                if(m_send_datas.empty())
                {
                    m_loop->Modify(this, EVENT_READ);
                    break;
                }
                const std::string& data = m_send_datas.front();
                m_send_ptr = (void*)data.data();
                m_send_size = data.size();
            }
            int ret = m_stream.Send(m_send_ptr, m_send_size);
            if(ret < 0)
            {
                m_loop->Unregister(this);
                return false;
            }
            else if(ret == 0)
            {
                break;
            }
            assert((size_t)m_send_size == 0);
            std::lock_guard<std::mutex> lock(m_mutex);
            assert(!m_send_datas.empty());
            m_send_datas.pop_front();
        }
        return true;
    }

    static bool CreateEventHandler(EventLoop* loop, Stream& stream, void* arg)
    {
        std::shared_ptr<EventHandler> handler =  std::make_shared<TestEventHandler>(loop, stream);
        return loop->Register(handler, EVENT_READ);
    }

private:
    enum{HEAD_SIZE=4};

    std::mutex m_mutex;

    char m_head[HEAD_SIZE];
    std::string m_pkt_data;

    void* m_head_ptr;
    ssize_t m_head_size;
    void* m_body_ptr;
    ssize_t m_body_size;

    std::deque<std::string> m_send_datas;
    void* m_send_ptr;
    ssize_t m_send_size;
};

void TestTimerHandlerCallback(void* timer, void* arg)
{
    //printf("timer handler called\n");
}

int main(int argc, char* argv[])
{
    time_t start_time = time(NULL);
    Address addr("127.0.0.1", PORT);

    TcpServer server(TestEventHandler::CreateEventHandler, nullptr);
    bool ret = server.Start(addr, 1024);
    if(!ret)
    {
        printf("cannot start server, errno: %d\n", errno);
        Thread::Sleep(1000);
        return 1;
    }

    TimerManager tm;
    tm.Start(2);

    void* timer = tm.AddTimer(500, TestTimerHandlerCallback);

    Thread::Sleep(10*1000);
	
    tm.RemoveTimer(timer);

    Thread::Sleep(10*1000);

    printf("server stopping: %ld\n", time(NULL));
    tm.Stop();
    server.Stop();
    
    printf("server start time:%ld, cost: %lds\n\n\n", start_time, time(NULL)-start_time);   

	return 0;
}

