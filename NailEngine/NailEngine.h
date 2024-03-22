#pragma once

#include <Windows.h>
#include <wrl.h>
#include <memory>
#include <vector>

#include "Utils.h"

class Device;
class SwapChain;
class ConstantBuffer;
class RenderTargetGroup;

struct WindowInfo
{
	HWND hWnd;
	unsigned int width;
	unsigned int height;
};

class NailEngine
{
public:
	static LazyObjects<NailEngine> Instance;
	friend LazyObjects<NailEngine>;

public:
	void Init(UINT64 hWnd);
	void Render();
	void Finalize();
public:
	void SetResolution(unsigned int width, unsigned int height);
	void SetUseIBL(bool useIBL);

	std::shared_ptr<ConstantBuffer>& GetConstantBuffer(unsigned int index);
	std::vector<std::shared_ptr<RenderTargetGroup>>& GetRenderTargetGroup() { return this->renderTargetGroup; }

	WindowInfo& GetWindowInfo() { return this->windowInfo; }
	bool GetUseIBL();

private:
	void CreateConstantBuffer();
	void CreateRenderTargetGroup();

private:
	WindowInfo windowInfo;

	std::shared_ptr<Device> device;
	std::shared_ptr<SwapChain> swapChain;

	std::vector<std::shared_ptr<ConstantBuffer>> constantBuffers;
	std::vector<std::shared_ptr<RenderTargetGroup>> renderTargetGroup;
	
	bool useIBL = true;
};

