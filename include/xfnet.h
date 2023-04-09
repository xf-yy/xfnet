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

#ifndef __xfnet_xfnet_h__
#define __xfnet_xfnet_h__

#include "xfnet/type.h"
#include "xfnet/address.h"
#include "xfnet/socket_util.h"
#include "xfnet/stream.h"
#include "xfnet/connector.h"
#include "xfnet/acceptor.h"
#include "xfnet/event_handler.h"
#include "xfnet/stream_handler.h"
#include "xfnet/selector.h"
#include "xfnet/thread.h"
#include "xfnet/rwlock.h"
#include "xfnet/event_loop.h"
#include "xfnet/tcp_client.h"
#include "xfnet/tcp_server.h"
#include "xfnet/timer_manager.h"

#if __cplusplus < 201103L
#error "only support c++ 11 or later, use -std=c++11 option for compile"
#endif

namespace xfnet 
{

//版本号字符串格式
const char* GetVersionString();

//版本号整型格式
uint32_t GetVersion();

} 

#endif

