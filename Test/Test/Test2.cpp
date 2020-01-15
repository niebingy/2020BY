// Test.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <iostream>

#include <WinSock2.h>
#include <windows.h>

#include "Thread.h"
#include "WorkerThreadPool.h"
#include "producer_consumer_queue.h"
#include "Api.h"
using namespace std;


producer_consumer_queue_ex<size_t> lp;
//������
class RunnableAdd : public IRunnable
{
public:
	void run()
	{
		for (size_t i = 0; i < 10000; i++)
		{
			lp.push_ex(i);
		}
	};

	void release(){};
};

//������
class RunnableGet : public IRunnable
{
public:
	void run()
	{
		size_t num = 0;
		for (size_t i = 0; i < 10000; i++)
		{
			lp.pop_ex(num);
		}
	};

	void release(){};
};


void Fun()
{
	const int gThreadId = 2;
	HANDLE hd[gThreadId];
	Thread *pThread = nullptr;

	for (size_t i = 0; i < 10; i++)
	{
		size_t num = 0;
		while (lp.pop_ex(num)) { }

		for (size_t i = 0; i < 10000; i++)
		{
			lp.push_ex(i); // �ȼ����ݣ���֤ÿ��ȡ���ݶ���
		}

		pThread = WorkerThreadPool::getInstancePtr()->add(new RunnableAdd);
		if (pThread == nullptr) { return; }
		hd[0] = pThread->GetHandle();

		pThread = WorkerThreadPool::getInstancePtr()->add(new RunnableGet);
		if (pThread == nullptr) { return; }
		hd[1] = pThread->GetHandle();

		DWORD result = ::WaitForMultipleObjects(gThreadId, hd, true, INFINITE);
		cout << "result: " << result << "count: " << lp.count() << endl;
	}

	cout << "����" << endl;
}

int _tmain(int argc, _TCHAR* argv[])
{
	new WorkerThreadPool;
	CreateReactor();

	Fun();
	system("pause");
	return 0;
}