/*******************************************************************
** �ļ���:	Reactor.h
** ��  ��:
** Ӧ  ��:
********************************************************************/
#pragma once
#include "Common.h"
#include "IReactor.h"
#include "singleton.h"
#include "SwappingList.h"
#include "Lock.h"


// ��ѯʱ����
#	define  POLLING_INTERVAL     1

/**
@name : ��Ӧ�� (Reactor)
@brief: ��Ӧ�����ǵȴ�����¼�Ȼ�������Ӧ�Ĵ���������ôһ��װ��
Windows����WaitForMultipleObjectsʵ��
@warning: ע��:
1.���/ɾ���¼���Ҫע����̼߳���
2.֧�ֶ���64���¼��ļ���,����64���¼�ʱʹ����ѯ��ʽ����¼�
*/
class Reactor : public IReactor, public EventHandler
	, public SingletonEx<Reactor>
{
public:
	/**
	@purpose    : ���Ҫ�������¼�
	@param event: �¼����
	@return		  : true  ... ע��ɹ�
	false ... �Ѿ��ﵽ����¼�����
	false ... ���¼�������
	*/
	virtual bool AddEvent(HANDLE event);

	/**
	@purpose    : �Ƴ�Ҫ�������¼�
	@param event: �¼����
	*/
	virtual void RemoveEvent(HANDLE event);

	/**
	@purpose      : ע��һ���¼� (һ��Handler���Դ������¼�,һ���¼������ɶ������������)
	@param event  : �¼����
	@param handler: �¼��������
	@return		  : true  ... ע��ɹ�
	false ... �Ѿ��ﵽ����¼�����
	false ... ���¼�������
	*/
	virtual bool RegisterEventHandler(HANDLE event, EventHandler * handler);

	/**
	@purpose      : ȡ��һ���¼�������
	@note         : �ô��������ٴ�������¼�����������¼����ɼ�����ֻ�е���RemoveEvent�Ų��ټ����¼�
	@param event  : �¼����
	@param handler: �¼��������
	@return		  :
	*/
	virtual void UnRegisterEventHandler(HANDLE event, EventHandler * handler);

	/**
	@purpose      : �ȴ��¼�����������
	@param	wait  : �ȴ��ĺ�����
	@return       : �ȴ��Ĺ������Ƿ����
	*/
	virtual bool HandleEvents(DWORD wait);

	/**
	@purpose      : ѭ���ȴ�/�����¼�
	*/
	virtual bool HandleEventsLoop();

	/**
	@name         : ֪ͨ��Ӧ���߳��˳�
	@brief        : ��Щ�������Ƕ��̰߳�ȫ��
	*/
	virtual void NotifyExit();

	/**
	@name         : ��������Ĳ����¼�,���������߳�ͬ��
	@brief        :
	*/
	virtual void OnEvent(HANDLE event);

	virtual void Release() { delete this; }

	Reactor();

	virtual ~Reactor();

protected:
	// �����Ĵ�����,�����Ѿ����˷�Ӧ���Լ����߳�,û�ж��߳�ͬ��������
	bool AddEventAux(HANDLE event);
	void RemoveEventAux(HANDLE event);
	bool RegisterEventHandlerAux(HANDLE event, EventHandler * handler);
	void UnRegisterEventHandlerAux(HANDLE event, EventHandler * handler);
	void NotifyExitAux();

protected:
	typedef std::list<EventHandler *>     HANDLER_LIST;
	typedef std::map<HANDLE, HANDLER_LIST> HANDLERS_MAP;

	HANDLE    *  m_EventHandles;                        // ���������¼�����
	DWORD        m_dwEventsNum;                         // �¼�����
	DWORD        m_dwArrayCapability;                   // ��������
	HANDLERS_MAP m_HandlersMap;                         // �¼��������б�

	HANDLE       m_hSelfEvent;                          // ��Ӧ���Լ���ά���¼�
	bool		 exit_flag;								// �˳����
	struct       REACTOR_TASK
	{
		enum
		{
			REACTOR_ADD = 0,
			REACTOR_DELETE,
			REACTOR_REGISTER,
			REACTOR_UNREGISTER,
			REACTOR_EXIT,
		};

		int      type;          // ��������
		HANDLE   event;
		EventHandler * handler;
	};

	SwappingList<REACTOR_TASK>   m_TaskList; // �����б�
	Mutex						 m_Mu;
};