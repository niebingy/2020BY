/*******************************************************************
** �ļ���:	Api.h
** ��  ��:
** Ӧ  ��:
********************************************************************/
#pragma once
#include "Common.h"
#include "IReactor.h"

/**
@name         : ����һ����Ӧ��
@brief        : ͨ��GetAsynIoReactor��õķ�Ӧ������IO�̼߳�����,���
			  : ����Ҫ�����¼��ֲ�����IO�߳̿����Լ�����һ��IReactorȥ����
*/
T20_API IReactor * CreateReactor();

/**
@name         : ȡ�������첽IO�ķ�Ӧ��ָ��
@brief        : ������Щ�ط������õõ�
*/
T20_API IReactor * GetReactor();