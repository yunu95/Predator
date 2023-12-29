#include "Mesh.h"

#include "ResourceBuilder.h"
#include "Device.h"

#include "NailEngine.h"
#include "RenderTargetGroup.h"

#include "InstanceBuffer.h"

void Mesh::SetData(std::vector<Vertex>& vertexVec, std::vector<unsigned int>& indexVec)
{
	VertexBuffer vertexBuffer;
	vertexBuffer.vertexCount = vertexVec.size();
	vertexBuffer.vertexVec = std::move(vertexVec);
	CreateVertexBuffer(vertexBuffer.vertexCount, vertexBuffer);
	this->vertexBufferVec.emplace_back(vertexBuffer);

	IndexBuffer indexBuffer;
	indexBuffer.indexCount = indexVec.size();
	indexBuffer.indexVec = std::move(indexVec);
	CreateIndexBuffer(indexBuffer.indexCount, indexBuffer);
	this->indexBufferVec.emplace_back(indexBuffer);

	this->materialCount = this->indexBufferVec.size();
}

void Mesh::Render(unsigned int materialIndex /*= 0*/, std::shared_ptr<InstanceBuffer> buffer /*= nullptr*/)
{
	if (buffer != nullptr)
	{
		unsigned int stride = sizeof(Vertex);
		unsigned int offset = 0;

		ResourceBuilder::Instance.Get().device->GetDeviceContext().Get()->IASetVertexBuffers(0, 1, this->vertexBufferVec[materialIndex].vertexBuffer.GetAddressOf(), &stride, &offset);
		ResourceBuilder::Instance.Get().device->GetDeviceContext().Get()->IASetIndexBuffer(this->indexBufferVec[materialIndex].indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		ResourceBuilder::Instance.Get().device->GetDeviceContext().Get()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		ResourceBuilder::Instance.Get().device->GetDeviceContext().Get()->DrawIndexedInstanced(this->indexBufferVec[materialIndex].indexCount, buffer->GetCount(), 0, 0, 0);
	}
	else
	{
		unsigned int stride = sizeof(Vertex);
		unsigned int offset = 0;

		ResourceBuilder::Instance.Get().device->GetDeviceContext().Get()->IASetVertexBuffers(0, 1, this->vertexBufferVec[materialIndex].vertexBuffer.GetAddressOf(), &stride, &offset);
		ResourceBuilder::Instance.Get().device->GetDeviceContext().Get()->IASetIndexBuffer(this->indexBufferVec[materialIndex].indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		ResourceBuilder::Instance.Get().device->GetDeviceContext().Get()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		ResourceBuilder::Instance.Get().device->GetDeviceContext().Get()->DrawIndexed(this->indexBufferVec[materialIndex].indexCount, 0, 0);
	}
}

void Mesh::CreateVertexBuffer(unsigned int vertexCount, VertexBuffer& vertexBuffer)
{
	D3D11_BUFFER_DESC _vertexBufferDesc;
	_vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	_vertexBufferDesc.ByteWidth = sizeof(Vertex) * vertexCount;
	_vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	_vertexBufferDesc.CPUAccessFlags = 0;
	_vertexBufferDesc.MiscFlags = 0;
	_vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA _vertexData;
	_vertexData.pSysMem = &(vertexBuffer.vertexVec[0]);
	_vertexData.SysMemPitch = 0;
	_vertexData.SysMemSlicePitch = 0;

	ResourceBuilder::Instance.Get().device->GetDevice().Get()->CreateBuffer(&_vertexBufferDesc, &_vertexData, vertexBuffer.vertexBuffer.GetAddressOf());
}

void Mesh::CreateIndexBuffer(unsigned int indexCount, IndexBuffer& indexBuffer)
{
	D3D11_BUFFER_DESC _indexBufferDesc;
	_indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	_indexBufferDesc.ByteWidth = sizeof(unsigned int) * indexCount;
	_indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	_indexBufferDesc.CPUAccessFlags = 0;
	_indexBufferDesc.MiscFlags = 0;
	_indexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA _indexData;
	_indexData.pSysMem = &(indexBuffer.indexVec[0]);
	_indexData.SysMemPitch = 0;
	_indexData.SysMemSlicePitch = 0;

	ResourceBuilder::Instance.Get().device->GetDevice().Get()->CreateBuffer(&_indexBufferDesc, &_indexData, indexBuffer.indexBuffer.GetAddressOf());
}
