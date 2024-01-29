#pragma once

#include <wrl.h>
#include <vector>

#include <d3d11.h>
#include <DirectXMath.h>
#include "SimpleMath.h"
using namespace DirectX::PackedVector;

struct InstancingData
{
	DirectX::SimpleMath::Matrix wtm;
};

#define MAX_INSTANCE 500

class InstanceBuffer
{
public:
	InstanceBuffer();
	~InstanceBuffer();

	void Create(const std::vector<InstancingData>& instanceDataVec, int slot);

	void AddData(InstancingData& data);

	void PushData();
	void ClearData();
	unsigned int GetCount() 
	{
		return static_cast<unsigned int>(_data.size()); 
	}

private:
	
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> _instanceBuffer;

	unsigned int _stride = 0;
	unsigned int _offset = 0;
	unsigned int _maxCount = 0;
	unsigned int _count = 0;
	unsigned int _slot = 1;


	std::vector<InstancingData> _data;
};

