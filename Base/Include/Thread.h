/*******************************************************************
** �ļ���:	Thread.h
** ��  ��:
** Ӧ  ��:
********************************************************************/
#pragma once
#include "Common.h"

//////////////////////////////////////////////////////////////////////////
/// �����ж���
struct IRunnable
{
	virtual void run() = 0;
	virtual void release() = 0;
};

/// ���̷߳�װ
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