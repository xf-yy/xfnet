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

#ifndef __xfnet_stream_handler_h__
#define __xfnet_stream_handler_h__

#include "xfnet.h"

using namespace xfutil;

namespace xfnet
{

class StreamHandler : public EventHandler
{
public:
    StreamHandler(EventLoop* loop, Stream& stream)
    : EventHandler(loop), m_stream(std::move(stream))
    {}

    virtual fd_t fd() const
    {
        return m_stream.fd();
    }
    
protected:
    Stream m_stream;

private:
	StreamHandler(const StreamHandler&) = delete;
	StreamHandler& operator=(const StreamHandler&) = delete;
};

typedef std::shared_ptr<StreamHandler> StreamHandlerPtr;
#define NewStreamHandler std::make_shared<StreamHandler>

} 

#endif

