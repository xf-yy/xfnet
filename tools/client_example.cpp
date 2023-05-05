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
#include "xfnet.h"

using namespace xfutil;
using namespace xfnet;

#define PORT        1981
#define TIMEOUT_MS  10000

//通讯协议[body_size][body]
//        4B         size
int main(int argc, char* argv[])
{
    time_t start_time = time(NULL);

    const int HEAD_SIZE = 4;
    const int MAX_SIZE = 4096;

    char buf[MAX_SIZE+HEAD_SIZE];
    char buf2[MAX_SIZE+HEAD_SIZE];
    
    Address addr("127.0.0.1", PORT);
    Stream stream = Connector::Connect(addr, 10*1000);
    if(!stream.Valid())
    {
        printf("connect failed, errno:%d\n", errno);
        Thread::Sleep(1000);
        return 1;
    }

    srand(time(NULL));
    for(int i = 0; i < MAX_SIZE + HEAD_SIZE; ++i)
    {
        buf[i] = rand();
    }
 
    const int MAX = 1024;
    int i;
    for(i = 0; i < MAX; ++i)
    {
        int len = rand() % MAX_SIZE;
        if(len < 512) len += 512;

        *(int*)buf = len;

        int wlen = stream.Send(buf, len+HEAD_SIZE, 10*1000);
        if(wlen == 0)
        {
            printf("send timeout\n");
            break;
        }
        else if(wlen != 1)
        {
            printf("send failed, errno:%d\n",  errno);
            break;
        }

        int rlen = stream.Recv(buf2, len+HEAD_SIZE, 10*1000);
        if(rlen == 0)
        {
            printf("recv timeout\n");
            break;
        }
        else if(rlen == -1)
        {
            printf("recv failed, errno:%d\n",  errno);
            break;
        }
        else if(rlen == -2)
        {
            printf("recv failed, remote client closed\n");
            break;
        }

        assert(memcmp(buf, buf2, len+HEAD_SIZE) == 0);
        memset(buf2, 0xcc, len+HEAD_SIZE);
    }	
    if(i < MAX)
        printf("test start time:%ld, cost: %lds\n\n\n", start_time, time(NULL)-start_time);   

    
	return 0;
}

