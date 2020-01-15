/*******************************************************************
** 文件名:	Singleton.h
** 描  述:
** 应  用:
********************************************************************/
#pragma once

#include <assert.h>
#include "Lock.h"

template<class T, bool mustDelete = true>
class Singleton
{
	static T*	_instance;		/// 实例静态指针
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
	/// 使用保护构造是为了用户不能在栈上声明一个实例
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

/// 静态实例指针初始化
template <class T, bool mustDelete> T* Singleton<T, mustDelete>::_instance = NULL;
template <class T, bool mustDelete> Mutex Singleton<T, mustDelete>::_mu;



/// 扩展的单实体模板，不关心对象的创建和销毁
/// 采用外部new和delete，这种单实体的好处是外部能控制构造和析构的顺序
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