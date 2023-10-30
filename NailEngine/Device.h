#pragma once

#include <wrl.h>
#include <d3d11.h>

class Device
{
public:
	~Device();
	void Init();

#pragma region Getter
	Microsoft::WRL::ComPtr<ID3D11Device>& GetDevice() { return this->device; }
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>& GetDeviceContext() { return deviceContext; }
#pragma endregion

private:
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;

};

