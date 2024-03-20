#pragma once
#include "IMesh.h"

#include "Resource.h"

#include <wrl.h>
#include <vector>
#include <d3d11.h>
#include <DirectXCollision.h>

#include "Struct.h"

struct VertexBuffer
{
    Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
    size_t vertexCount;
    std::vector<Vertex> vertexVec;
};

struct IndexBuffer
{
    Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
    size_t indexCount;
    std::vector<unsigned int> indexVec;
};

class InstanceBuffer;

class Mesh : public yunuGI::IMesh, public Resource
{
public:
    void SetData(std::vector<Vertex>& vertexVec, std::vector<unsigned int>& indexVec, DirectX::SimpleMath::Vector3& maxPoint, DirectX::SimpleMath::Vector3& minPoint);

    void Render(unsigned int materialIndex = 0, std::shared_ptr<InstanceBuffer> buffer = nullptr);

    virtual void GetBoundingBoxInfo(const yunuGI::Matrix4x4& wtm, yunuGI::Vector3* min, yunuGI::Vector3* max);

#pragma region Getter
    unsigned int GetMaterialCount() { return this->materialCount; }
    std::vector<VertexBuffer>& GetVertex() { return  vertexBufferVec; }
    std::vector<IndexBuffer>& GetIdx() { return indexBufferVec; }
	DirectX::BoundingBox GetBoundingBox(DirectX::SimpleMath::Matrix wtm, unsigned int materialIndex = 0);
    //virtual void GetBoundingBoxInfo(yunuGI::Vector3* min, yunuGI::Vector3* max);
	std::vector<DirectX::SimpleMath::Vector3>& GetBoundingVertexList(DirectX::SimpleMath::Matrix& mat, int index);

#pragma endregion

private:
    void CreateVertexBuffer(unsigned int vertexCount, VertexBuffer& vertexBuffer);
    void CreateIndexBuffer(unsigned int indexCount, IndexBuffer& indexBuffer);

    void CheckBigBoundingBox(DirectX::BoundingBox& aabb);
	

private:
    std::vector<VertexBuffer> vertexBufferVec;
    std::vector<IndexBuffer> indexBufferVec;

	DirectX::BoundingBox aabb;
	std::vector<DirectX::BoundingBox> aabbVec;
	std::vector<std::vector<DirectX::SimpleMath::Vector3>> boundingVertexList;
	std::vector<std::vector<DirectX::SimpleMath::Vector3>> tempBoundingVertexList;

    unsigned int materialCount;

};

