/*******************************************************************
** �ļ���:	SwappingList.h 
** ��  ��:
** Ӧ  ��:
********************************************************************/
#pragma once
#include "Common.h"
#include "Lock.h"

/**
@name : ��ת����
@brief: �ڶ��߳�������-������ģʽ���������߳��������Ӷ����������̴߳Ӷ���ȡ�߶���
        ��Ϊ�漰�����̷߳��ʣ�������б��������/ȡ�߶���ʱ���������ģ��ʵ�����ֶ��߳�
		��ȫ�Ķ��У�����ʹ���������з�ת�ļ��������Լ���ļ���������ļ��ʡ�

		���ж���߳�������ݣ�һ���߳�ȡ���ݡ�ȡ�����߳̿���ȥ��������ʱ�����ַ�������������ˡ�
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

// �Է�ת���н��з�װ���Լ������Ƿ���Ҫ���м���
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