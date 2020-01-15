/*******************************************************************
** �ļ���:	WorkerThreadPool.h 
** ��  ��:
** Ӧ  ��:
********************************************************************/
#pragma once

#include "Common.h"
#include "singleton.h"
#include "ThreadPool.h"

/**
@name : �����߳�����
@brief: 
*/
class WorkerThreadPool : public ThreadPool,public SingletonEx<WorkerThreadPool>
{
	typedef std::list<IRunnable*>	WorkerList;
	WorkerList m_WorkerList;
public:
	~WorkerThreadPool()
	{
		for (WorkerList::iterator it = m_WorkerList.begin(); it != m_WorkerList.end(); ++it)
			(*it)->release();
		m_WorkerList.clear();
	}

	Thread * add(IRunnable* task, int priority = 0)
	{
		m_WorkerList.push_back(task);
		return ThreadPool::add(task, priority);
	}
};