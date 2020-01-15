/*******************************************************************
** 文件名:	Api.cpp
** 描  述:
** 应  用:
********************************************************************/
#pragma once
#include "stdafx.h"
#include "Api.h"
#include "Reactor.h"
/**
@name         : 创建一个反应器
@brief        : 通过GetAsynIoReactor获得的反应器是在IO线程监听的,如果
: 你需要监听事件又不想在IO线程可以自己创建一个IReactor去监听
*/
T20_API IReactor * CreateReactor()
{
	return new Reactor;
}

/**
@name         : 取得用于异步IO的反应器指针
@brief        : 其他有些地方可以用得到
*/
T20_API IReactor * GetReactor()
{
	return Reactor::getInstancePtr();
}