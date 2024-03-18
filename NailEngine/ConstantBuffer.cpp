#include "ConstantBuffer.h"

#include <guiddef.h>
#include <assert.h>

#include "ResourceBuilder.h"
#include "Device.h"

#include "ILight.h"

ConstantBuffer::ConstantBuffer()
	: mappedBuffer{nullptr}
{

}

ConstantBuffer::~ConstantBuffer()
{
	delete[] mappedBuffer;
}

void ConstantBuffer::CraeteConstantBuffer(unsigned int size)
{
	this->size = (size + 15) & ~15;

	mappedBuffer = new BYTE[this->size];

	D3D11_BUFFER_DESC _bufferDesc;
	_bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	//_bufferDesc.ByteWidth = this->size * this->count;
	_bufferDesc.ByteWidth = this->size;
	_bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	_bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	_bufferDesc.MiscFlags = 0;
	_bufferDesc.StructureByteStride = 0;
	
	//HRESULT _hr = ::CoCreateGuid(&myGuid);

	ResourceBuilder::Instance.Get().device->GetDevice()->CreateBuffer(&_bufferDesc, nullptr, this->buffer.GetAddressOf());
	//buffer.Get()->SetPrivateData(myGuid, this->size * this->count, this->mappedBuffer);
}

void ConstantBuffer::PushGraphicsData(void* data, unsigned int size, unsigned int slot, bool useGS)
{
	assert(this->size == ((size + 15) & ~15));

	D3D11_MAPPED_SUBRESOURCE _subResource;

	ResourceBuilder::Instance.Get().device->GetDeviceContext()->Map(this->buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &_subResource);
	::memcpy(_subResource.pData, data, size);
	ResourceBuilder::Instance.Get().device->GetDeviceContext()->Unmap(this->buffer.Get(), 0);

	ResourceBuilder::Instance.Get().device->GetDeviceContext()->VSSetConstantBuffers(slot, 1, this->buffer.GetAddressOf());
	ResourceBuilder::Instance.Get().device->GetDeviceContext()->PSSetConstantBuffers(slot, 1, this->buffer.GetAddressOf());

	if (useGS)
	{
		ResourceBuilder::Instance.Get().device->GetDeviceContext()->GSSetConstantBuffers(slot, 1, this->buffer.GetAddressOf());
	}
}

void ConstantBuffer::Clear()
{

}
