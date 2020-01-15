/*******************************************************************
** �ļ���:	producer_consumer_queue.h
** ��  ��:
** Ӧ  ��:
********************************************************************/

#pragma once
#include "Trace.h"
#include "Common.h"
#include "MemPool.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
��ν�������������߶��У���һ���̶߳�һ���߳�д�Ķ���
��������ʵ�ֵ���һ�������������������߶���,�㷨�Ĺؼ���:

1.popֻ�޸�ͷ�ڵ�,pushֻ�޸�β�ڵ�
2.��֤head��tail�ǿ�,��Ϊһ��head��tailΪ��,pushʱ�ͱ���ͬʱ��дhead��tail

�ر�˵��:
1.�������������߶���������ͬʱֻ����һ���̶߳���һ���߳�д
2.�������������߳������,��ÿһʱ��ֻ����һ��������,ͬ�����������д�߳�����д,��ÿһʱ��ֻ����һ��д,�����û��ο������ʵ��
*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename _DATA>
class producer_consumer_queue
{
	struct NODE
	{
		_DATA   data;
		NODE *  next;
	};

	NODE *  m_pHead;
	NODE *  m_pTail;

	volatile unsigned long    m_nPushCount;
	volatile unsigned long    m_nPopCount;
	tstring m_strName;	// ���ƣ�����������ʾӵ����

public:
	producer_consumer_queue() : m_nPushCount(0), m_nPopCount(0)
	{
		//m_pHead = new NODE;
		m_pHead = (NODE*)MT_Allocator::getInstance().allocate(sizeof(NODE));
		assert(m_pHead);

		// ��֤ Head �� Tail �ǿ�
		m_pHead->next = 0;
		m_pTail = m_pHead;
	};

	virtual ~producer_consumer_queue()
	{
		_DATA temp_data;
		while (pop(temp_data)) {}

		if (m_pHead)
		{
			delete m_pHead;
			m_pHead = 0;
			m_pTail = 0;
		}
	}


	// ͬʱֻ����һ���߳�push
	void push(_DATA & data)
	{
		//NODE * pNode = new NODE;
		NODE * pNode = (NODE*)MT_Allocator::getInstance().allocate(sizeof(NODE));
		pNode->data = data;
		pNode->next = 0;

		m_pTail->next = pNode;
		m_pTail = pNode;

		++m_nPushCount;

		static DWORD s_dwTick = 0;
		if (m_nPushCount - m_nPopCount > 1024 && GetTickCount() - s_dwTick > 10000)
		{
			s_dwTick = GetTickCount();
			DWORD dwPackCounts = m_nPushCount - m_nPopCount;
			if (dwPackCounts > 100000)
			{
				WarningLn(0, "[" << m_strName.c_str() << "]����æ, ѹջ" << m_nPushCount << "��, ��ջ" << m_nPopCount << "��, ջ����" << dwPackCounts << ", ���ݳ���" << sizeof(_DATA) << "�ֽ�");
			}
			else
			{
				WarningLn(0, "[" << m_strName.c_str() << "]����æ, ѹջ" << m_nPushCount << "��, ��ջ" << m_nPopCount << "��, ջ����" << dwPackCounts << ", ���ݳ���" << sizeof(_DATA) << "�ֽ�");
			}
		}

	}

	// ͬʱֻ����һ���߳�pop
	bool pop(_DATA & data)
	{
		if (m_pHead->next != 0)
		{
			++m_nPopCount;

			NODE * pNode = m_pHead;
			m_pHead = m_pHead->next;
			data = m_pHead->data;
			//delete pNode;
			MT_Allocator::getInstance().deallocate(pNode);

			return true;
		}

		return false;
	}

	unsigned long count()
	{
		return m_nPushCount - m_nPopCount;
	}

public:

	bool setName(const tchar * pstrName)
	{
		if (pstrName == nullptr) { return false; }

		m_strName = pstrName;
		return true;
	}
	const tchar * getName()
	{
		return m_strName.c_str();
	}
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
����дȨ�����Ƶ������������߶���
��:���������߳������,����֮ǰ��Ҫȡ�ö�Ȩ��,Ҳ������д�߳�����д,��д֮ǰ��Ҫȡ��дȨ��
�÷�:
1.ֱ�ӵ���push_ex��pop_ex
2.�Լ�����acquire_read_permission��pop�Ⱥ���,��Ҫע���̻߳����߼�
*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename _DATA>
class producer_consumer_queue_ex : public producer_consumer_queue<_DATA>
{
	volatile	LONG  m_nReadBlock;		// ����־
	volatile    LONG  m_nWriteBlock;    // д��־

public:
	producer_consumer_queue_ex() : producer_consumer_queue(), m_nReadBlock(0), m_nWriteBlock(0)
	{
	}

	// ��Ȩ����д
	bool push_ex(_DATA & data)
	{
		AtomLock __lock__(&m_nWriteBlock);
		producer_consumer_queue::push(data);
		return true;
	}

	// ��Ȩ���Ķ�
	bool pop_ex(_DATA & data)
	{
		if (count() == 0)  // ��һ���жϣ���ֹ����ĵ���ԭ�Ӳ�����η�����ڴ�����
		{
			return false;
		}

		AtomLock __lock__(&m_nReadBlock);
		return producer_consumer_queue::pop(data);
	}
};


