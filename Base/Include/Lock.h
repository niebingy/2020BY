#pragma once
#include "Common.h"
#include <windows.h>

//////////////////////////////////////////////////////////////////////////
// 互斥体l
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
/// 临界区线程安全自动保护
template<class _Mutex = Mutex>
class ResGuard
{
	_Mutex& mMutex;
public:
	ResGuard(_Mutex& mtx) : mMutex(mtx)		{ mMutex.Lock(); }
	~ResGuard()								{ mMutex.Unlock(); }
};

// 临界区自动锁
struct AutoLock
{
	inline AutoLock(LPCRITICAL_SECTION cs) :_cs(cs){ EnterCriticalSection(_cs); }
	inline ~AutoLock(){ LeaveCriticalSection(_cs); }
	LPCRITICAL_SECTION _cs;
};

/*

LONG InterlockedCompareExchange( LONG volatile* Destination, LONG Exchange, LONG Comparand );
是把目标操作数（第1参数所指向的内存中的数）与一个值（第3参数）比较，
如果相等，则用另一个值（第2参数）与目标操作数（第1参数所指向的内存中的数）交换；
InterlockedExchange是不比较直接交换。整个操作过程是锁定内存的，其它处理器不会同时访问内存，从而实现多处理器环境下的线程互斥。
*/

// 原子自动锁
struct AtomLock
{
	inline AtomLock(long volatile * v) :_var(v){ while (1 == InterlockedCompareExchange(_var, 1, 0)){ Sleep(0); } }
	inline ~AtomLock(){ *_var = 0; }
	long volatile * _var;
};

// 互斥量自动锁
struct AutoMutex
{
	inline AutoMutex(Mutex * m) :_mutex(m){ _mutex->Lock(); }
	inline ~AutoMutex(){ _mutex->Unlock(); }
	Mutex * _mutex;
};
