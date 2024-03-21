#include "Mesh.h"

#include "ResourceBuilder.h"
#include "Device.h"

#include "NailEngine.h"
#include "NailCamera.h"
#include "RenderTargetGroup.h"
#include "ConstantBuffer.h"
#include "CameraManager.h"
#include "ResourceManager.h"

#include "InstanceBuffer.h"

void Mesh::SetData(std::vector<Vertex>& vertexVec, std::vector<unsigned int>& indexVec, DirectX::SimpleMath::Vector3& maxPoint, DirectX::SimpleMath::Vector3& minPoint)
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

	DirectX::BoundingBox aabb;
	aabb.Center = DirectX::SimpleMath::Vector3((maxPoint.x + minPoint.x) * 0.5f, (maxPoint.y + minPoint.y) * 0.5f, (maxPoint.z + minPoint.z) * 0.5f);
	aabb.Extents = DirectX::SimpleMath::Vector3((maxPoint.x - minPoint.x) * 0.5f, (maxPoint.y - minPoint.y) * 0.5f, (maxPoint.z - minPoint.z) * 0.5f);
	this->aabbVec.emplace_back(aabb);


	std::vector<DirectX::SimpleMath::Vector3> tempVertexList;
	tempVertexList.push_back(DirectX::SimpleMath::Vector3{ minPoint.x, minPoint.y, minPoint.z }); // [0]
	tempVertexList.push_back(DirectX::SimpleMath::Vector3{ maxPoint.x, minPoint.y, minPoint.z });
	tempVertexList.push_back(DirectX::SimpleMath::Vector3{ minPoint.x, maxPoint.y, minPoint.z });
	tempVertexList.push_back(DirectX::SimpleMath::Vector3{ maxPoint.x, maxPoint.y, minPoint.z });
	tempVertexList.push_back(DirectX::SimpleMath::Vector3{ minPoint.x, minPoint.y, maxPoint.z });
	tempVertexList.push_back(DirectX::SimpleMath::Vector3{ maxPoint.x, minPoint.y, maxPoint.z });
	tempVertexList.push_back(DirectX::SimpleMath::Vector3{ minPoint.x, maxPoint.y, maxPoint.z });
	tempVertexList.push_back(DirectX::SimpleMath::Vector3{ maxPoint.x, maxPoint.y, maxPoint.z }); // [7]
	this->boundingVertexList.push_back(tempVertexList);
	this->tempBoundingVertexList = this->boundingVertexList;

    CheckBigBoundingBox(aabb);

    this->materialCount = this->indexBufferVec.size();
}

void Mesh::SetDiffuseExposure(float exposure)
{
	this->diffuseExposure = exposure;
}

void Mesh::SetAmbientExposure(float exposure)
{
	this->ambientExposure = exposure;
}

void Mesh::Render(unsigned int materialIndex /*= 0*/, std::shared_ptr<InstanceBuffer> buffer /*= nullptr*/)
{
	if (buffer != nullptr)
	{
		unsigned int stride = sizeof(Vertex);
		unsigned int offset = 0;

		ResourceBuilder::Instance.Get().device->GetDeviceContext()->IASetVertexBuffers(0, 1, this->vertexBufferVec[materialIndex].vertexBuffer.GetAddressOf(), &stride, &offset);
		ResourceBuilder::Instance.Get().device->GetDeviceContext()->IASetIndexBuffer(this->indexBufferVec[materialIndex].indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		ResourceBuilder::Instance.Get().device->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		ResourceBuilder::Instance.Get().device->GetDeviceContext()->DrawIndexedInstanced(this->indexBufferVec[materialIndex].indexCount, buffer->GetCount(), 0, 0, 0);
	}
	else
	{
		unsigned int stride = sizeof(Vertex);
		unsigned int offset = 0;

		ResourceBuilder::Instance.Get().device->GetDeviceContext()->IASetVertexBuffers(0, 1, this->vertexBufferVec[materialIndex].vertexBuffer.GetAddressOf(), &stride, &offset);
		ResourceBuilder::Instance.Get().device->GetDeviceContext()->IASetIndexBuffer(this->indexBufferVec[materialIndex].indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		ResourceBuilder::Instance.Get().device->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		ResourceBuilder::Instance.Get().device->GetDeviceContext()->DrawIndexed(this->indexBufferVec[materialIndex].indexCount, 0, 0);
	}
}

DirectX::BoundingBox Mesh::GetBoundingBox(DirectX::SimpleMath::Matrix wtm, unsigned int materialIndex /*= 0*/)
{
	DirectX::BoundingBox transformedAABB;

	XMStoreFloat3(&transformedAABB.Center, DirectX::XMLoadFloat3(&this->aabbVec[materialIndex].Center));
	XMStoreFloat3(&transformedAABB.Extents, DirectX::XMLoadFloat3(&this->aabbVec[materialIndex].Extents));

    transformedAABB.Transform(transformedAABB, wtm);

	return transformedAABB;
}
void Mesh::GetBoundingBoxInfo(const yunuGI::Matrix4x4& wtm, yunuGI::Vector3* min, yunuGI::Vector3* max)
{
    auto aabb = GetBoundingBox(reinterpret_cast<const DirectX::SimpleMath::Matrix&>(wtm), 0);
    if (min)
    {
        *min = yunuGI::Vector3(aabb.Center.x - aabb.Extents.x, aabb.Center.y - aabb.Extents.y, aabb.Center.z - aabb.Extents.z);
    }
    if (max)
    {
        *max = yunuGI::Vector3(aabb.Center.x + aabb.Extents.x, aabb.Center.y + aabb.Extents.y, aabb.Center.z + aabb.Extents.z);
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

	ResourceBuilder::Instance.Get().device->GetDevice()->CreateBuffer(&_vertexBufferDesc, &_vertexData, vertexBuffer.vertexBuffer.GetAddressOf());
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

	ResourceBuilder::Instance.Get().device->GetDevice()->CreateBuffer(&_indexBufferDesc, &_indexData, indexBuffer.indexBuffer.GetAddressOf());
}

void Mesh::CheckBigBoundingBox(DirectX::BoundingBox& aabb)
{
    float v1 = this->aabb.Extents.x * this->aabb.Extents.y * this->aabb.Extents.z;
    float v2 = aabb.Extents.x * aabb.Extents.y * aabb.Extents.z;

    if (v1 < v2)
    {
        this->aabb = aabb;
    }
}

std::vector<DirectX::SimpleMath::Vector3>& Mesh::GetBoundingVertexList(DirectX::SimpleMath::Matrix& mat, int index)
{
	for (int i = 0; i < this->boundingVertexList[index].size(); ++i)
	{
		this->tempBoundingVertexList[index][i] = XMVector3Transform(this->boundingVertexList[index][i], mat);
	}

	return this->tempBoundingVertexList[index];
}

float Mesh::GetDiffuseExposure()
{
	return this->diffuseExposure;
}

float Mesh::GetAmbientExposure()
{
	return this->ambientExposure;
}
