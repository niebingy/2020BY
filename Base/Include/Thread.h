/*******************************************************************
** 文件名:	Thread.h
** 描  述:
** 应  用:
********************************************************************/
#pragma once
#include "Common.h"

//////////////////////////////////////////////////////////////////////////
/// 可运行对象
struct IRunnable
{
	virtual void run() = 0;
	virtual void release() = 0;
};

/// 简单线程封装
class T20_EXPORT Thread
{
protected:
	ulong	m_ThreadId;
	HANDLE	m_ThreadHandle;

public:
	Thread();
	~Thread();

public:
	bool spawn(IRunnable* task, int priority = 0);
	void wait();
	void start();
	void pause();
	void terminate();
	void sleep(unsigned long timeout);
	HANDLE GetHandle();

protected:
	static unsigned int __stdcall _dispatch(void* arg);
};