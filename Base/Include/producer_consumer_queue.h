/*******************************************************************
** 文件名:	producer_consumer_queue.h
** 描  述:
** 应  用:
********************************************************************/

#pragma once
#include "Trace.h"
#include "Common.h"
#include "MemPool.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
所谓的生产者消费者队列，即一个线程读一个线程写的队列
我们这里实现的是一个无锁的生产者消费者队列,算法的关键点:

1.pop只修改头节点,push只修改尾节点
2.保证head和tail非空,因为一旦head和tail为空,push时就必须同时改写head和tail

特别说明:
1.该生成者消费者队列理论上同时只允许一个线程读和一个线程写
2.可以允许多个读线程申请读,但每一时刻只允许一个真正读,同理可以允许多个写线程申请写,但每一时刻只允许一个写,具体用户参考下面的实现
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
	tstring m_strName;	// 名称，用来调试显示拥有者

public:
	producer_consumer_queue() : m_nPushCount(0), m_nPopCount(0)
	{
		//m_pHead = new NODE;
		m_pHead = (NODE*)MT_Allocator::getInstance().allocate(sizeof(NODE));
		assert(m_pHead);

		// 保证 Head 和 Tail 非空
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


	// 同时只允许一个线程push
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
				WarningLn(0, "[" << m_strName.c_str() << "]队列忙, 压栈" << m_nPushCount << "次, 出栈" << m_nPopCount << "次, 栈长度" << dwPackCounts << ", 数据长度" << sizeof(_DATA) << "字节");
			}
			else
			{
				WarningLn(0, "[" << m_strName.c_str() << "]队列忙, 压栈" << m_nPushCount << "次, 出栈" << m_nPopCount << "次, 栈长度" << dwPackCounts << ", 数据长度" << sizeof(_DATA) << "字节");
			}
		}

	}

	// 同时只允许一个线程pop
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
带读写权利控制的生产者消费者队列
即:允许多个读线程申请读,但读之前需要取得读权利,也允许多个写线程申请写,但写之前需要取得写权利
用法:
1.直接调用push_ex和pop_ex
2.自己调用acquire_read_permission和pop等函数,但要注意线程互斥逻辑
*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename _DATA>
class producer_consumer_queue_ex : public producer_consumer_queue<_DATA>
{
	volatile	LONG  m_nReadBlock;		// 读标志
	volatile    LONG  m_nWriteBlock;    // 写标志

public:
	producer_consumer_queue_ex() : producer_consumer_queue(), m_nReadBlock(0), m_nWriteBlock(0)
	{
	}

	// 带权利的写
	bool push_ex(_DATA & data)
	{
		AtomLock __lock__(&m_nWriteBlock);
		producer_consumer_queue::push(data);
		return true;
	}

	// 带权利的读
	bool pop_ex(_DATA & data)
	{
		if (count() == 0)  // 加一个判断，防止过多的调用原子操作无畏的锁内存总线
		{
			return false;
		}

		AtomLock __lock__(&m_nReadBlock);
		return producer_consumer_queue::pop(data);
	}
};


