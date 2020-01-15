/*******************************************************************
** �ļ���:	Singleton.h
** ��  ��:
** Ӧ  ��:
********************************************************************/
#pragma once

#include <assert.h>
#include "Lock.h"

template<class T, bool mustDelete = true>
class Singleton
{
	static T*	_instance;		/// ʵ����ָ̬��
	static Mutex _mu;
public:
	static T& getInstance()
	{
		if (!_instance)
		{
			ResGuard<Mutex> lock(_mu);
			if (!_instance)
			{
				_instance = new T;
				if (mustDelete) atexit(releaseInstance);
			}
		}

		return *_instance;
	}

protected:
	/// ʹ�ñ���������Ϊ���û�������ջ������һ��ʵ��
	Singleton() { }
	static void __cdecl releaseInstance()
	{
		if (_instance && mustDelete)
		{
			delete _instance;
			_instance = 0;
		}
	}
};

/// ��̬ʵ��ָ���ʼ��
template <class T, bool mustDelete> T* Singleton<T, mustDelete>::_instance = NULL;
template <class T, bool mustDelete> Mutex Singleton<T, mustDelete>::_mu;



/// ��չ�ĵ�ʵ��ģ�壬�����Ķ���Ĵ���������
/// �����ⲿnew��delete�����ֵ�ʵ��ĺô����ⲿ�ܿ��ƹ����������˳��
template <typename T>
class SingletonEx
{
protected:
	static T*	_instance;

public:
	SingletonEx()
	{
		assert(!_instance);
		_instance = static_cast<T*>(this);
	}

	~SingletonEx()
	{
		assert(_instance);
		_instance = 0;
	}

	static T& getInstance()		{ assert(_instance); return (*_instance); }
	static T* getInstancePtr()	{ return _instance; }
};

template <typename T> T* SingletonEx<T>::_instance = 0;