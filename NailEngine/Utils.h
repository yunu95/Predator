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

enum 
{
	SWAP_CHAIN_BUFFER_COUNT = 2,
	G_BUFFER_MEMBER_COUNT = 4,
	LIGHTING_MEMBER_COUNT =2,
};

enum
{
	POSITION,
	NORMAL,
	ALBEDO,
	DEPTH,
	DIFFUSE = 0,
	SPECULAR = 1,
	GBUFFER_COUNT
};

enum class RENDER_TARGET_TYPE
{
	//SWAP_CHAIN,
	G_BUFFER,
	LIGHTING,
	END
};