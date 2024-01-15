#include "InstanceBuffer.h"

#include "ResourceBuilder.h"
#include "Device.h"

InstanceBuffer::InstanceBuffer()
{
	_maxCount = MAX_INSTANCE;
	std::vector<InstancingData> temp(MAX_INSTANCE);
	Create(temp, 1);
}

InstanceBuffer::~InstanceBuffer()
{

}

void InstanceBuffer::Create(const std::vector<InstancingData>& instanceDataVec, int slot)
{
	_stride = sizeof(InstancingData);
	_count = static_cast<int>(instanceDataVec.size());
	_maxCount = _count;
	_slot = slot;

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.ByteWidth = (int)(_stride * _count);

	desc.Usage = D3D11_USAGE_DYNAMIC; // CPU Write, GPU Read
	desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(data));
	data.pSysMem = instanceDataVec.data();

	ResourceBuilder::Instance.Get().device->GetDevice()->CreateBuffer(&desc, &data, _instanceBuffer.GetAddressOf());
}

void InstanceBuffer::AddData(InstancingData& data)
{
	_data.push_back(data);
}

void InstanceBuffer::PushData()
{
	const unsigned int dataCount = GetCount();
	if (dataCount > _maxCount)
		Create(_data, dataCount);

	D3D11_MAPPED_SUBRESOURCE subResource;

	ResourceBuilder::Instance.Get().device->GetDeviceContext()->Map(_instanceBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	{
		::memcpy(subResource.pData, _data.data(), sizeof(InstancingData) * dataCount);
	}
	ResourceBuilder::Instance.Get().device->GetDeviceContext()->Unmap(_instanceBuffer.Get(), 0);

	ResourceBuilder::Instance.Get().device->GetDeviceContext()->IASetVertexBuffers(1, 1, _instanceBuffer.GetAddressOf(), &_stride, &_offset);
}

void InstanceBuffer::ClearData()
{
	_data.clear();
}
