#pragma once

#include <wrl.h>
#include <d3d11.h>

class Device
{
public:
	~Device();
	void Init(void* device, void* deviceContext);

#pragma region Getter
	ID3D11Device*& GetDevice() { return this->device; }
	ID3D11DeviceContext*& GetDeviceContext() { return deviceContext; }
#pragma endregion

private:
	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;

};

