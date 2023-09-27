#pragma once

#include <wrl.h>

#include <d3d11.h>

class ConstantBuffer
{
public:
	ConstantBuffer();
	~ConstantBuffer();

	void CraeteConstantBuffer(unsigned int size, unsigned int count);

	void PushGraphicsData(void* data, unsigned int size);

	void Clear();

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;

	GUID myGuid;

	unsigned int size;
	unsigned int count;
	unsigned int currentIndex;

	BYTE* mappedBuffer;
};

