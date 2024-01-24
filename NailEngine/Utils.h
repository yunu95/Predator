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
	G_BUFFER_MEMBER_COUNT = 5,
	LIGHTING_MEMBER_COUNT = 2,
	IRRADIANCE_MEMBER_COUNT = 1,
	SPECIBL_MEMBER_COUNT = 1,
};

enum
{
	POSITION,
	NORMAL,
	ALBEDO,
	DEPTH,
	ARM,
	DIFFUSE = 0,
	SPECULAR = 1,
	IRRADIANCE = 0,
	SPECIBL = 0,
	GBUFFER_COUNT
};

enum class RENDER_TARGET_TYPE
{
	//SWAP_CHAIN,
	G_BUFFER,
	LIGHTING,
	IRRADIANCE,
	SPECLIBL,
	END
};