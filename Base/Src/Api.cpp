/*******************************************************************
** �ļ���:	Api.cpp
** ��  ��:
** Ӧ  ��:
********************************************************************/
#pragma once
#include "stdafx.h"
#include "Api.h"
#include "Reactor.h"
/**
@name         : ����һ����Ӧ��
@brief        : ͨ��GetAsynIoReactor��õķ�Ӧ������IO�̼߳�����,���
: ����Ҫ�����¼��ֲ�����IO�߳̿����Լ�����һ��IReactorȥ����
*/
T20_API IReactor * CreateReactor()
{
	return new Reactor;
}

/**
@name         : ȡ�������첽IO�ķ�Ӧ��ָ��
@brief        : ������Щ�ط������õõ�
*/
T20_API IReactor * GetReactor()
{
	return Reactor::getInstancePtr();
}