#pragma once

#include <wrl.h>
#include <vector>

#include <d3d11.h>

#include "Struct.h"

class Mesh
{
public:
	void Init(const std::vector<Vertex>& vertexVec, const std::vector<unsigned int>& indexVec);

	void Render();

#pragma region Setter
	void SetMeshName(const std::wstring& meshName) { this->meshName = meshName; }

#pragma endregion

private:
	void CreateVertexBuffer();
	void CreateIndexBuffer();

private:
	std::wstring meshName;

	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;

	size_t vertexCount;
	size_t indexCount;

	std::vector<Vertex> vertexVec;
	std::vector<unsigned int> indexVec;
};

