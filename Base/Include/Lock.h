#pragma once
#include "Common.h"
#include <windows.h>

//////////////////////////////////////////////////////////////////////////
// ������l
class Mutex
{
	CRITICAL_SECTION m_cs;

public:
	Mutex() { ::InitializeCriticalSection(&m_cs); };
	~Mutex() { ::DeleteCriticalSection(&m_cs); };
	void Lock() { ::EnterCriticalSection(&m_cs); };
	void Unlock() { ::LeaveCriticalSection(&m_cs); };
};

//////////////////////////////////////////////////////////////////////////
/// �ٽ����̰߳�ȫ�Զ�����
template<class _Mutex = Mutex>
class ResGuard
{
	_Mutex& mMutex;
public:
	ResGuard(_Mutex& mtx) : mMutex(mtx)		{ mMutex.Lock(); }
	~ResGuard()								{ mMutex.Unlock(); }
};

// �ٽ����Զ���
struct AutoLock
{
	inline AutoLock(LPCRITICAL_SECTION cs) :_cs(cs){ EnterCriticalSection(_cs); }
	inline ~AutoLock(){ LeaveCriticalSection(_cs); }
	LPCRITICAL_SECTION _cs;
};

/*

LONG InterlockedCompareExchange( LONG volatile* Destination, LONG Exchange, LONG Comparand );
�ǰ�Ŀ�����������1������ָ����ڴ��е�������һ��ֵ����3�������Ƚϣ�
�����ȣ�������һ��ֵ����2��������Ŀ�����������1������ָ����ڴ��е�����������
InterlockedExchange�ǲ��Ƚ�ֱ�ӽ������������������������ڴ�ģ���������������ͬʱ�����ڴ棬�Ӷ�ʵ�ֶദ���������µ��̻߳��⡣
*/

// ԭ���Զ���
struct AtomLock
{
	inline AtomLock(long volatile * v) :_var(v){ while (1 == InterlockedCompareExchange(_var, 1, 0)){ Sleep(0); } }
	inline ~AtomLock(){ *_var = 0; }
	long volatile * _var;
};

// �������Զ���
struct AutoMutex
{
	inline AutoMutex(Mutex * m) :_mutex(m){ _mutex->Lock(); }
	inline ~AutoMutex(){ _mutex->Unlock(); }
	Mutex * _mutex;
};
