#pragma once

#include "Utils.h"
#include "Struct.h"
#include "Animation.h"

#include <memory>
#include <string>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <vector>

#include <DirectXMath.h>
#include "SimpleMath.h"
using namespace DirectX::PackedVector;

struct BoneInfo
{
	int index = -1;
	DirectX::SimpleMath::Matrix offset;
};

struct MaterialData
{
	std::wstring materialName;
	std::wstring albedoMap;
	std::wstring normalMap;
	std::wstring armMap;
	std::wstring emissionMap;
	std::wstring opacityMap;
	std::wstring heightMap;

	std::wstring temp0Map;
	std::wstring temp1Map;
	std::wstring temp2Map;
	std::wstring temp3Map;

	std::wstring vs{L"DefaultVS.cso"};
	std::wstring ps{ L"DefaultPS.cso" };
};

struct FBXMeshData
{
	std::wstring meshName;

	MaterialData material;

	DirectX::SimpleMath::Vector3 aabb[2];
	std::vector<Vertex> vertex;
	std::vector<unsigned int> indices;
};

struct FBXNode
{ 
	std::wstring nodeName;
	std::vector<FBXNode*> child;
	std::vector<FBXMeshData> meshVec;
	DirectX::SimpleMath::Matrix transformMatrix;
	DirectX::SimpleMath::Matrix worldMatrix;
	bool hasAnimation = false;
};

