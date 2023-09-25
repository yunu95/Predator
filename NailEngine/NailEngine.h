#pragma once

#include <Windows.h>
#include <wrl.h>
#include <memory>




#include "Utils.h"

class Device;
class SwapChain;

class NailEngine
{
public:
	static LazyObjects<NailEngine> Instance;
	friend LazyObjects<NailEngine>;

public:
	void Init(UINT64 hWnd);
	void Render();

public:
	void SetResolution(unsigned int width, unsigned int height);

private:
	HWND hWnd;
	unsigned int width;
	unsigned int height;

	std::shared_ptr<Device> device;
	std::shared_ptr<SwapChain> swapChain;

	

};

