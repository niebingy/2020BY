/*******************************************************************
** 文件名:	Reactor.cpp
** 描  述:
** 应  用:
********************************************************************/

#include "stdafx.h"
#include "Reactor.h"
#include "Trace.h"

//////////////////////////////////////////////////////////////////////////
Reactor::Reactor()
{
	m_dwEventsNum = 0;
	exit_flag = FALSE;
	m_dwArrayCapability = MAXIMUM_WAIT_OBJECTS;
	m_EventHandles = new HANDLE[m_dwArrayCapability];
	memset(m_EventHandles, 0, m_dwArrayCapability*sizeof(HANDLE));

	m_hSelfEvent = ::CreateEvent(NULL, FALSE, FALSE, 0);
	AddEventAux(m_hSelfEvent);
	RegisterEventHandlerAux(m_hSelfEvent, this);
}

//////////////////////////////////////////////////////////////////////////
Reactor::~Reactor()
{
	if (m_EventHandles)
	{
		delete[]m_EventHandles;
		m_EventHandles = 0;
	}

	if (m_hSelfEvent != INVALID_HANDLE_VALUE)
	{
		::CloseHandle(m_hSelfEvent);
		m_hSelfEvent = INVALID_HANDLE_VALUE;
	}
}

//////////////////////////////////////////////////////////////////////////
bool Reactor::AddEventAux(HANDLE event)
{
	// 扩容
	if (m_dwEventsNum >= m_dwArrayCapability)
	{
		m_dwArrayCapability += MAXIMUM_WAIT_OBJECTS;
		HANDLE * newArray = new HANDLE[m_dwArrayCapability];
		memcpy(newArray, m_EventHandles, m_dwEventsNum*sizeof(HANDLE));
		delete[]m_EventHandles;
		m_EventHandles = newArray;
	}

	for (DWORD i = 0; i < m_dwEventsNum; ++i)
	{
		if (m_EventHandles[i] == event)
		{
			return false;
		}
	}

	m_EventHandles[m_dwEventsNum++] = event;
	return true;
}

//////////////////////////////////////////////////////////////////////////
void Reactor::RemoveEventAux(HANDLE event)
{
	for (DWORD i = 0; i < m_dwEventsNum; ++i)
	{
		if (m_EventHandles[i] == event)
		{
			m_EventHandles[i] = 0;
		}
	}

	m_HandlersMap.erase(event);
}

//////////////////////////////////////////////////////////////////////////
bool Reactor::RegisterEventHandlerAux(HANDLE event, EventHandler * handler)
{
	HANDLER_LIST & handlerList = m_HandlersMap[event];
	HANDLER_LIST::iterator it = std::find(handlerList.begin(), handlerList.end(), handler);
	if (it != handlerList.end())
		return false;

	handlerList.push_back(handler);
	handler->OnRegisterOK(event);
	return true;
}

//////////////////////////////////////////////////////////////////////////
void Reactor::UnRegisterEventHandlerAux(HANDLE event, EventHandler * handler)
{
	HANDLER_LIST & handlerList = m_HandlersMap[event];
	HANDLER_LIST::iterator it = std::find(handlerList.begin(), handlerList.end(), handler);
	if (it != handlerList.end())
	{
		handlerList.erase(it);
		handler->OnUnRegisterOK(event);
	}
}

//////////////////////////////////////////////////////////////////////////
void Reactor::NotifyExitAux()
{
	//::ExitThread(0);
	exit_flag = true;
}

//////////////////////////////////////////////////////////////////////////
bool Reactor::HandleEvents(DWORD wait)
{
	// 如果事件个数大于64就应该是polling模式,这时候不应该是INFINITE的等待时间
	if (m_dwEventsNum > MAXIMUM_WAIT_OBJECTS)
	{
		if (wait == INFINITE)
		{
			wait = POLLING_INTERVAL;
		}
	}

	// 统一清理已删除的事件
	for (DWORD i = 0; i < m_dwEventsNum; ++i)
	{
		if (m_EventHandles[i] == 0)
		{
			if (i < m_dwEventsNum - 1)
			{
				memmove(&m_EventHandles[i], &m_EventHandles[i + 1], (m_dwEventsNum - i - 1)*sizeof(HANDLE));
			}

			--m_dwEventsNum;
		}
	}

	// 没有事件就休眠
	if (m_dwEventsNum == 0)
	{
		::Sleep(1);
		return true;
	}

	DWORD offset = 0;

	// 开始轮询
	do
	{
		ulong wait_num = min(m_dwEventsNum - offset, MAXIMUM_WAIT_OBJECTS);
		DWORD result = ::WaitForMultipleObjects(wait_num, m_EventHandles + offset, FALSE, wait);
		if (result == WAIT_TIMEOUT)
		{
			offset += wait_num;
			continue;
		}

		if (result == WAIT_FAILED)
		{
			// 应用层删除事件,因为Reactor的线程可能还在等待,
			// 所以由于事件句柄已释放,这里可能会产生一个错误,待删除后就会正常了

			OnEvent(m_hSelfEvent);
			continue;
		}

		DWORD index = result - WAIT_OBJECT_0;
		if (index >= 0 && index < wait_num)
		{
			// 派发给事件处理器
			HANDLER_LIST & handlerList = m_HandlersMap[m_EventHandles[index + offset]];
			HANDLER_LIST::iterator it = handlerList.begin();
			for (; it != handlerList.end(); ++it)
			{
				EventHandler * handler = *it;
				handler->OnEvent(m_EventHandles[index + offset]);

				// 只要触发了一轮就一定要设置为POLLING模式，否则后面如果死循环监听就会出问题
				wait = POLLING_INTERVAL;
			}

			// 如果是第一个事件表示有事件新增或减少,需要重新监听一下
			if (index + offset == 0)
			{
				return !exit_flag;
			}
		}
		else
		{
			TraceLn(0, "Reactor::HandleEvents error" << GetLastError());  // 奇怪的返回值
			return true;
		}
		//继续检查下一组
		offset += wait_num;

	} while (!exit_flag && offset < m_dwEventsNum);

	return !exit_flag;
}

//////////////////////////////////////////////////////////////////////////
bool Reactor::HandleEventsLoop()
{
	// 大于64个事件自动转为轮询模式
	while (HandleEvents(m_dwEventsNum > MAXIMUM_WAIT_OBJECTS ? POLLING_INTERVAL : INFINITE))
	{
	}

	return true;
}


//////////////////////////////////////////////////////////////////////////
bool Reactor::AddEvent(HANDLE event)
{
	REACTOR_TASK task;
	task.event = event;
	task.type = REACTOR_TASK::REACTOR_ADD;
	task.handler = 0;
	m_TaskList.Add(task);
	SetEvent(m_hSelfEvent);
	return true;
}

//////////////////////////////////////////////////////////////////////////
void Reactor::RemoveEvent(HANDLE event)
{
	REACTOR_TASK task;
	task.event = event;
	task.type = REACTOR_TASK::REACTOR_DELETE;
	task.handler = 0;

	m_TaskList.Add(task);
	SetEvent(m_hSelfEvent);
	return;
}

//////////////////////////////////////////////////////////////////////////
bool Reactor::RegisterEventHandler(HANDLE event, EventHandler * handler)
{
	REACTOR_TASK task;
	task.event = event;
	task.type = REACTOR_TASK::REACTOR_REGISTER;
	task.handler = handler;

	m_TaskList.Add(task);
	SetEvent(m_hSelfEvent);
	return true;
}

//////////////////////////////////////////////////////////////////////////
void Reactor::UnRegisterEventHandler(HANDLE event, EventHandler * handler)
{
	REACTOR_TASK task;
	task.event = event;
	task.type = REACTOR_TASK::REACTOR_UNREGISTER;
	task.handler = handler;

	m_TaskList.Add(task);
	SetEvent(m_hSelfEvent);
	return;
}

//////////////////////////////////////////////////////////////////////////
void Reactor::NotifyExit()
{
	REACTOR_TASK task;
	task.event = 0;
	task.type = REACTOR_TASK::REACTOR_EXIT;
	task.handler = 0;

	m_TaskList.Add(task);
	SetEvent(m_hSelfEvent);
	return;
}

//////////////////////////////////////////////////////////////////////////
void Reactor::OnEvent(HANDLE event)
{
	if (event == INVALID_HANDLE_VALUE)
		return;

	REACTOR_TASK task;
	while (m_TaskList.Get(task))
	{
		switch (task.type)
		{
		case REACTOR_TASK::REACTOR_ADD:
			AddEventAux(task.event);
			break;
		case REACTOR_TASK::REACTOR_DELETE:
			RemoveEventAux(task.event);
			break;
		case REACTOR_TASK::REACTOR_REGISTER:
			RegisterEventHandlerAux(task.event, task.handler);
			break;
		case REACTOR_TASK::REACTOR_UNREGISTER:
			UnRegisterEventHandlerAux(task.event, task.handler);
			break;
		case REACTOR_TASK::REACTOR_EXIT:
			NotifyExitAux();
			break;
		default:
			assert(false);
			break;
		}
	}
}