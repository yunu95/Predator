#pragma once
#define _HAS_STD_BYTE 0

#include <Windows.h>
#include <string>

template <class T>
class LazyObjects
{
public:
	T& Get()
	{
		if (!instance)
			instance = new T();
		return *instance;
	}

	void Release()
	{
		if (!instance)
			return;

		delete instance;
		instance = nullptr;
	}

private:
	T* instance = nullptr;
};