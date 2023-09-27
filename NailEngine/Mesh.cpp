#include "Mesh.h"

#include "ResourceBuilder.h"
#include "Device.h"

void Mesh::Init(const std::vector<Vertex>& vertexVec, const std::vector<unsigned int>& indexVec)
{
	this->vertexCount = vertexVec.size();
	this->indexCount = indexVec.size();

	this->vertexVec  = vertexVec;
	this->indexVec = indexVec;

	CreateVertexBuffer();
	CreateIndexBuffer();
}

void Mesh::Render()
{
	unsigned int stride = sizeof(MatrixBuffer);
	unsigned int offset = 0;

	ResourceBuilder::Instance.Get().device->GetDeviceContext().Get()->IASetVertexBuffers(0, 1, this->vertexBuffer.GetAddressOf(), &stride, &offset);
	ResourceBuilder::Instance.Get().device->GetDeviceContext().Get()->IASetIndexBuffer(this->indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	ResourceBuilder::Instance.Get().device->GetDeviceContext().Get()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	ResourceBuilder::Instance.Get().device->GetDeviceContext().Get()->DrawIndexed(indexCount, 0, 0);
}

void Mesh::CreateVertexBuffer()
{
	D3D11_BUFFER_DESC _vertexBufferDesc;
	_vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	_vertexBufferDesc.ByteWidth = sizeof(MatrixBuffer) * this->vertexCount;
	_vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	_vertexBufferDesc.CPUAccessFlags = 0;
	_vertexBufferDesc.MiscFlags = 0;
	_vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA _vertexData;
	_vertexData.pSysMem = &this->vertexVec;
	_vertexData.SysMemPitch = 0;
	_vertexData.SysMemSlicePitch = 0;

	ResourceBuilder::Instance.Get().device->GetDevice().Get()->CreateBuffer(&_vertexBufferDesc, &_vertexData, this->vertexBuffer.GetAddressOf());
}

void Mesh::CreateIndexBuffer()
{
	D3D11_BUFFER_DESC _indexBufferDesc;
	_indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	_indexBufferDesc.ByteWidth = sizeof(unsigned int) * this->indexCount;
	_indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	_indexBufferDesc.CPUAccessFlags = 0;
	_indexBufferDesc.MiscFlags = 0;
	_indexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA _indexData;
	_indexData.pSysMem = &this->indexVec;
	_indexData.SysMemPitch = 0;
	_indexData.SysMemSlicePitch = 0;

	ResourceBuilder::Instance.Get().device->GetDevice().Get()->CreateBuffer(&_indexBufferDesc, &_indexData, this->indexBuffer.GetAddressOf());
}
