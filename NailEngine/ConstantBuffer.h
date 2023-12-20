#pragma once

#include <wrl.h>

#include <d3d11.h>

class ConstantBuffer
{
public:
	ConstantBuffer();
	~ConstantBuffer();

	void CraeteConstantBuffer(unsigned int size);

	void PushGraphicsData(void* data, unsigned int size, unsigned int slot);

	void Clear();

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;

	GUID myGuid;

	unsigned int size;

	BYTE* mappedBuffer;
};

