#include "Device.h"

Device::~Device()
{
	device->Release();
	deviceContext->Release();
}

void Device::Init()
{
	UINT createDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if defined(_DEBUG) || defined(DEBUG)
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	HRESULT _hr = S_FALSE;

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_11_1,
	};

	_hr = ::D3D11CreateDevice(
		nullptr,	// 기본 어댑터
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,	// 소프트웨어 장치를 사용하지 않음
		createDeviceFlags,
		featureLevels,
		ARRAYSIZE(featureLevels),
		D3D11_SDK_VERSION,
		device.GetAddressOf(),
		nullptr,
		deviceContext.GetAddressOf()
	);
}