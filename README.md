# ●项目介绍   
***   
    xfnet是一个基于reactor模式开发的网络库，采用c++11语言开发。   
    作者：xiaofan <75631302@qq.com>   
    
   
# ●编译方法   
***   
    注：当前版本仅支持linux环境编译，且gcc版本最低4.8.5版本，编译方法如下：   
    mkdir build   
    cd build    
    cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=./ ..   
    make   
    make install   
   
# ●使用样例   
***   
 
```  
    //client样例
    Address addr("127.0.0.1", 1981); 

    Stream stream = Connector::Connect(addr, 10*1000);
    if(!stream.Valid())
    {
        printf("connect failed, errno:%d\n", errno);
        return 1;
    }

    char buf[1024];
    char buf2[1024];
    {
        //通讯协议[body_size][body]
        //        4B         size
        int len = 1000;
        *(int*)buf = len;

        int wlen = stream.Send(buf, len+4, 10*1000);
        assert(wlen == 1);

        int rlen = stream.Recv(buf2, len+4, 10*1000);
        assert(rlen == 1);

        assert(memcmp(buf, buf2, len+4) == 0);
    }	

    //server端样例参考tools\server_exapmple.cpp
```  

   
# ●版权信息   
***   
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
   
