// Test.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include "WorkerThreadPool.h"
#include "Api.h"
using namespace std;

class RunnableReactor : public IRunnable
{
public:
	void run()
	{
		GetReactor()->HandleEventsLoop();
	};

	void release(){};
};

class Runnable : public IRunnable, public EventHandler
{
public:
	void run()
	{
		HANDLE hEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
		GetReactor()->AddEvent(hEvent);
		GetReactor()->RegisterEventHandler(hEvent, this);
		SetEvent(hEvent);
	};
	
	void OnEvent(HANDLE event)
	{
		cout << "Runnable OnEvent 线程id: " << GetCurrentThreadId() << " event:" << event << endl;
	}

	void release(){};
};

void ThreadFun()
{
	for (size_t i = 0; i < 10; i++)
	{
		WorkerThreadPool::getInstancePtr()->add(new Runnable);
	}
	
	Sleep(1000);
	cout << "..." << endl;
}

void EndFun()
{
	HANDLE hEvent = ::CreateEvent(NULL, FALSE, FALSE, L"EndF");
	::WaitForSingleObject(hEvent, INFINITE);
	::CloseHandle(hEvent);
}

int _tmain2(int argc, _TCHAR* argv[])
{
	new WorkerThreadPool;
	CreateReactor();
	WorkerThreadPool::getInstancePtr()->add(new RunnableReactor);

	ThreadFun();

	EndFun();
	return 0;
}



