/*******************************************************************
** �ļ���:	IReactor.h
** ��  ��:
** Ӧ  ��:
********************************************************************/
#pragma once

/// �¼��������
struct EventHandler;

/**
@name : ��Ӧ���ӿ� (Reactor)
@brief: ��Ӧ�����ǵȴ�����¼�Ȼ�������Ӧ�Ĵ���������ôһ��װ��
Windows����WaitForMultipleObjectsʵ��
*/
struct IReactor
{
	/**
	@name       : ���Ҫ�������¼�
	@brief      : ��Щ�������Ƕ��̰߳�ȫ��
	@param event: �¼����
	@return		  : true  ... ע��ɹ�
	false ... �Ѿ��ﵽ����¼�����
	false ... ���¼�������
	*/
	virtual bool AddEvent(HANDLE event) = 0;

	/**
	@name       : �Ƴ�Ҫ�������¼�
	@brief      : ��Щ�������Ƕ��̰߳�ȫ��
	@param event: �¼����
	*/
	virtual void RemoveEvent(HANDLE event) = 0;

	/**
	@name         : ע��һ���¼� (һ��Handler���Դ������¼�,һ���¼������ɶ������������)
	@brief        : ��Щ�������Ƕ��̰߳�ȫ��
	@param event  : �¼����
	@param handler: �¼��������
	@return		  : true  ... ע��ɹ�
	false ... �Ѿ��ﵽ����¼�����
	false ... ���¼�������
	*/
	virtual bool RegisterEventHandler(HANDLE event, EventHandler * handler) = 0;

	/**
	@name         : ȡ��һ���¼�������
	@brief        : ��Щ�������Ƕ��̰߳�ȫ��
	@note         : �ô��������ٴ�������¼�����������¼����ɼ�����ֻ�е���RemoveEvent�Ų��ټ����¼�
	@param event  : �¼����
	@param handler: �¼��������
	@return		  :
	*/
	virtual void UnRegisterEventHandler(HANDLE event, EventHandler * handler) = 0;

	/**
	@purpose      : �ȴ��¼�����������
	@param	wait  : �ȴ��ĺ�����
	@return       : �ȴ��Ĺ������Ƿ����
	*/
	virtual bool HandleEvents(DWORD wait) = 0;

	/**
	@purpose      : ѭ���ȴ�/�����¼�
	*/
	virtual bool HandleEventsLoop() = 0;

	/**
	@name         : ֪ͨ��Ӧ���߳��˳�
	@brief        : ��Щ�������Ƕ��̰߳�ȫ��
	*/
	virtual void NotifyExit() = 0;

	virtual void Release() = 0;
};

/**
@name : �¼�������
@brief:
*/
struct EventHandler
{
	virtual void OnEvent(HANDLE event) = 0;

	// ֪ͨע��/ע���ɹ���ע�⣬���ڷ�Ӧ���߳�����ص�����ӿ�
	virtual void OnRegisterOK(HANDLE event) {}
	virtual void OnUnRegisterOK(HANDLE event) {}
};