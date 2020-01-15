/*******************************************************************
** 文件名:	SwappingList.h 
** 描  述:
** 应  用:
********************************************************************/
#pragma once
#include "Common.h"
#include "Lock.h"

/**
@name : 翻转队列
@brief: 在多线程生产者-消费者模式中生产者线程向队列添加对象，消费者线程从队列取走对象
        因为涉及到多线程访问，这个队列必须在添加/取走对象时加锁。这个模版实现这种多线程
		安全的队列，另外使用两个队列翻转的技术，可以极大的减少锁互斥的几率。

		当有多个线程添加数据，一个线程取数据。取数据线程可以去掉锁，此时用这种方法处理是最好了。
*/

template<typename _OBJ>
class SwappingList
{
	Mutex				  m_frontMutex;
	Mutex				  m_backMutex;
public:
	size_t Add(_OBJ & obj )
	{
		::InterlockedIncrement(&m_nCount);
		
		m_frontList->push_back(obj);
		return m_frontList->size();
	}

	bool Get(_OBJ & obj )
	{
		if ( m_nCount == 0 ) { return false; }
		static unsigned long s_last_trace = 0;

		if ( m_nCount > 1000 && GetTickCount() - s_last_trace > 10000 )
		{
			s_last_trace = GetTickCount();
			TraceLn(0, "The queues blocked sizeof(_OBJ):" << sizeof(_OBJ) <<" m_nCount:" << m_nCount);
		}

		

		if ( m_backList->empty() ) { Swap(); }
		if ( m_backList->empty() ) { return false; }

		::InterlockedDecrement(&m_nCount);
		obj = m_backList->front();
		m_backList->pop_front();
		return true;
	}

	int Count() 
	{
		return m_nCount;
	}

	void Swap()
	{
		ResGuard<Mutex> lock(m_frontMutex);
		PRODUCT_LIST * p = m_backList;
		m_backList = m_frontList;
		m_frontList = p;
	}
	
	SwappingList()
	{
		m_frontList = new PRODUCT_LIST;
		m_backList  = new PRODUCT_LIST;

		m_nCount = 0;
	}

	virtual ~SwappingList()
	{
		if ( m_frontList )
		{
			delete m_frontList;
			m_frontList = 0;
		}

		if ( m_backList )
		{
			delete m_backList;
			m_backList = 0;
		}
	}

protected:
	typedef  std::list<_OBJ>   PRODUCT_LIST;
	PRODUCT_LIST *        m_frontList;
	PRODUCT_LIST *        m_backList;


	volatile LONG         m_nCount;
};

// 对翻转队列进行封装，自己决定是否需要进行加锁
template <typename _DATA>
class SwappingListEx : public SwappingList<_DATA>
{
public:
	size_t AddEx(_DATA & obj)
	{
		ResGuard<Mutex> lock(m_frontMutex);
		return SwappingList::Add(obj);
	}

	bool Get(_DATA & obj)
	{
		ResGuard<Mutex> lock(m_backMutex); 
		return SwappingList::Get(_OBJ & obj);
	}
};