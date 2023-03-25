/*************************************************************************
Copyright (C) 2023 The xfdb Authors. All rights reserved.

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

#ifndef __xfutil_thread_h__
#define __xfutil_thread_h__

#include <thread>
#include <mutex>
#include <vector>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include "xfnet/type.h"

namespace xfutil
{

typedef void (*ThreadFunc)(void* arg);	

class Thread
{
public:
	Thread()
	{}
		
public:
	bool Start(ThreadFunc func, void* arg = nullptr)
	{
		std::thread t(func, arg);
		m_thread.swap(t);
        return true;
	}
	void Detach()
	{
		m_thread.detach();
	}
	void Join()
	{
		m_thread.join();
	}

	static void Sleep(uint32_t time_ms)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(time_ms));
	}
	static void Yield()
	{
		std::this_thread::yield();
	}
	static inline threadid_t ID()
	{
		return syscall(SYS_gettid);
	}
	
private:
	std::thread m_thread;
	
private:
	Thread(const Thread&) = delete;
	Thread& operator=(const Thread&) = delete;
};

typedef void (*GThreadFunc)(int index, void* arg);	

class ThreadGroup
{
public:
	ThreadGroup()
	{
	}
		
public:
	void Start(int thread_count, GThreadFunc func, void* arg = nullptr);
	void Detach();
	void Join();
	
private:
	mutable std::mutex m_mutex;
	std::vector<std::thread> m_threads;
	
private:
  ThreadGroup(const ThreadGroup&) = delete;
  ThreadGroup& operator=(const ThreadGroup&) = delete;
		
};

}

#endif

