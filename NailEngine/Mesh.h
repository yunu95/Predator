#pragma once
#include "IMesh.h"

#include "Resource.h"

#include <wrl.h>
#include <vector>
#include <d3d11.h>
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
	void SetData(std::vector<Vertex>& vertexVec, std::vector<unsigned int>& indexVec);

	void Render(std::shared_ptr<InstanceBuffer> buffer = nullptr, unsigned int materialIndex = 0);


#pragma region Getter
	unsigned int GetMaterialCount() { return this->materialCount; }
	std::vector<VertexBuffer>& GetVertex() { return  vertexBufferVec; }
	std::vector<IndexBuffer>& GetIdx() { return indexBufferVec; }

#pragma endregion

private:
	void CreateVertexBuffer(unsigned int vertexCount, VertexBuffer& vertexBuffer);
	void CreateIndexBuffer(unsigned int indexCount,IndexBuffer& indexBuffer);

private:
	std::vector<VertexBuffer> vertexBufferVec;
	std::vector<IndexBuffer> indexBufferVec;

	unsigned int materialCount;
	
};

