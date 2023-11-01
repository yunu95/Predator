#include "ConstantBuffer.h"

#include <guiddef.h>
#include <assert.h>

#include "ResourceBuilder.h"
#include "Device.h"

#include "ILight.h"

ConstantBuffer::ConstantBuffer()
	: mappedBuffer{nullptr},
	currentIndex{0}
{

}

ConstantBuffer::~ConstantBuffer()
{
	delete[] mappedBuffer;
}

void ConstantBuffer::CraeteConstantBuffer(unsigned int size, unsigned int count)
{
	this->size = (size + 15) & ~15;
	this->count = count;

	mappedBuffer = new BYTE[this->size * this->count];

	D3D11_BUFFER_DESC _bufferDesc;
	_bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	//_bufferDesc.ByteWidth = this->size * this->count;
	_bufferDesc.ByteWidth = this->size;
	_bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	_bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	_bufferDesc.MiscFlags = 0;
	_bufferDesc.StructureByteStride = 0;
	
	//HRESULT _hr = ::CoCreateGuid(&myGuid);

	ResourceBuilder::Instance.Get().device->GetDevice().Get()->CreateBuffer(&_bufferDesc, nullptr, this->buffer.GetAddressOf());
	//buffer.Get()->SetPrivateData(myGuid, this->size * this->count, this->mappedBuffer);
}

void ConstantBuffer::PushGraphicsData(void* data, unsigned int size, unsigned int slot)
{
	//assert(this->currentIndex < this->count);
	assert(this->size == ((size + 15) & ~15));

	D3D11_MAPPED_SUBRESOURCE _subResource;

	ResourceBuilder::Instance.Get().device->GetDeviceContext().Get()->Map(this->buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &_subResource);
	//::memcpy(&this->mappedBuffer[this->currentIndex * this->size], data, size);
	::memcpy(_subResource.pData, data, size);
	ResourceBuilder::Instance.Get().device->GetDeviceContext().Get()->Unmap(this->buffer.Get(), 0);

	ResourceBuilder::Instance.Get().device->GetDeviceContext().Get()->VSSetConstantBuffers(slot, 1, this->buffer.GetAddressOf());
	ResourceBuilder::Instance.Get().device->GetDeviceContext().Get()->PSSetConstantBuffers(slot, 1, this->buffer.GetAddressOf());

	//this->currentIndex++;
}

void ConstantBuffer::Clear()
{
	this->currentIndex = 0;
}
