/*******************************************************************
** �ļ���:	ThreadPool.h
** ��  ��:
** Ӧ  ��:
********************************************************************/
#pragma once

#include "Common.h"
#include "Thread.h"

//////////////////////////////////////////////////////////////////////////
// �̳߳�
class ThreadPool
{
protected:
	typedef std::vector<Thread*>	ThreadList;
	ThreadList	m_Threads;
	bool m_AutoRemove;	/// �������߳�ִ�н������Զ�ɾ��

public:
	ThreadPool::ThreadPool(void) {}
	ThreadPool::~ThreadPool(void) { clear(); }

	Thread * ThreadPool::add(IRunnable* task, int priority = 0)
	{
		Thread* thread = new Thread();
		if (!thread->spawn(task, priority))
		{
			return NULL;
		}

		m_Threads.push_back(thread);
		return thread;
	}

	void ThreadPool::start()
	{
		for (ThreadList::iterator it = m_Threads.begin(); it != m_Threads.end(); ++it)
		{
			(*it)->start();
		}
	}

	void ThreadPool::pause()
	{
		for (ThreadList::iterator it = m_Threads.begin(); it != m_Threads.end(); ++it)
		{
			(*it)->pause();
		}
	}

	void ThreadPool::wait()
	{
		for (ThreadList::iterator it = m_Threads.begin(); it != m_Threads.end(); ++it)
		{
			(*it)->wait();
		}
	}

	void ThreadPool::clear()
	{
		for (ThreadList::iterator it = m_Threads.begin(); it != m_Threads.end(); ++it)
		{
			delete *it;
		}

		m_Threads.clear();
	}

	void ThreadPool::remove(Thread * thread)
	{
		for (ThreadList::iterator it = m_Threads.begin(); it != m_Threads.end(); ++it)
		{
			if (thread == (*it))
			{
				delete *it;
				m_Threads.erase(it);
				return;
			}
		}
	}

	size_t getThreadCount() const	{ return m_Threads.size(); }
};
