/*******************************************************************
** 文件名:	IReactor.h
** 描  述:
** 应  用:
********************************************************************/
#pragma once

/// 事件处理程序
struct EventHandler;

/**
@name : 反应器接口 (Reactor)
@brief: 反应器就是等待多个事件然后调用相应的处理程序的这么一个装置
Windows下用WaitForMultipleObjects实现
*/
struct IReactor
{
	/**
	@name       : 填加要监听的事件
	@brief      : 这些函数都是多线程安全的
	@param event: 事件句柄
	@return		  : true  ... 注册成功
	false ... 已经达到最大事件个数
	false ... 该事件不存在
	*/
	virtual bool AddEvent(HANDLE event) = 0;

	/**
	@name       : 移除要监听的事件
	@brief      : 这些函数都是多线程安全的
	@param event: 事件句柄
	*/
	virtual void RemoveEvent(HANDLE event) = 0;

	/**
	@name         : 注册一个事件 (一个Handler可以处理多个事件,一个事件可以由多个处理器处理)
	@brief        : 这些函数都是多线程安全的
	@param event  : 事件句柄
	@param handler: 事件处理程序
	@return		  : true  ... 注册成功
	false ... 已经达到最大事件个数
	false ... 该事件不存在
	*/
	virtual bool RegisterEventHandler(HANDLE event, EventHandler * handler) = 0;

	/**
	@name         : 取消一个事件处理器
	@brief        : 这些函数都是多线程安全的
	@note         : 该处理器不再处理这个事件，但是这个事件依旧监听，只有调用RemoveEvent才不再监听事件
	@param event  : 事件句柄
	@param handler: 事件处理程序
	@return		  :
	*/
	virtual void UnRegisterEventHandler(HANDLE event, EventHandler * handler) = 0;

	/**
	@purpose      : 等待事件触发并分派
	@param	wait  : 等待的毫秒数
	@return       : 等待的过程中是否出错
	*/
	virtual bool HandleEvents(DWORD wait) = 0;

	/**
	@purpose      : 循环等待/分派事件
	*/
	virtual bool HandleEventsLoop() = 0;

	/**
	@name         : 通知反应器线程退出
	@brief        : 这些函数都是多线程安全的
	*/
	virtual void NotifyExit() = 0;

	virtual void Release() = 0;
};

/**
@name : 事件处理器
@brief:
*/
struct EventHandler
{
	virtual void OnEvent(HANDLE event) = 0;

	// 通知注册/注销成功，注意，是在反应器线程里面回调这个接口
	virtual void OnRegisterOK(HANDLE event) {}
	virtual void OnUnRegisterOK(HANDLE event) {}
};