/*******************************************************************
** 文件名:	Thread.cpp
** 描  述:
** 应  用:
********************************************************************/
#include "stdafx.h"
#include "Thread.h"
#include <process.h>

//////////////////////////////////////////////////////////////////////////
// 线程封装
Thread::Thread() : m_ThreadId(0), m_ThreadHandle(0)
{
}

Thread::~Thread()
{
	if ( m_ThreadHandle !=0 )
	{
		::TerminateThread(m_ThreadHandle,0);
		m_ThreadHandle = 0;
	}
}

bool Thread::spawn(IRunnable* task, int priority) 
{
	m_ThreadHandle = (HANDLE)::_beginthreadex(0, 0, &_dispatch, task, CREATE_SUSPENDED, (unsigned int*)&m_ThreadId);
	::SetThreadPriority(m_ThreadHandle, priority);
	::ResumeThread(m_ThreadHandle);
	return m_ThreadHandle != NULL;
}

void Thread::wait()
{
	if (m_ThreadHandle == NULL) { return; }
	::WaitForSingleObject(m_ThreadHandle, INFINITE);

	m_ThreadHandle = NULL;
}

void Thread::start()
{
	::ResumeThread(m_ThreadHandle);
}

void Thread::pause()
{
	::SuspendThread(m_ThreadHandle);
}

void Thread::terminate()
{
	::TerminateThread(m_ThreadHandle, 0);
	::CloseHandle(m_ThreadHandle);
	m_ThreadHandle = NULL;
}

void Thread::sleep(unsigned long timeout)
{
	Sleep(timeout);
}

unsigned int __stdcall Thread::_dispatch(void* arg)
{
	IRunnable* task = reinterpret_cast<IRunnable*>(arg);
	assert(task);

	task->run();
	return 0;
}

HANDLE Thread::GetHandle()
{
	return m_ThreadHandle;
}