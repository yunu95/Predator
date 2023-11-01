#pragma once
#include "YunuGraphicsInterface.h"

#include "Utils.h"
#include "Struct.h"

#include <memory>
#include <string>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <vector>

#include <DirectXMath.h>
#include "SimpleMath.h"
using namespace DirectX::PackedVector;

class Mesh;



struct FBXMaterialData
{
	std::wstring materialName;
	std::wstring albedoMap;
	std::wstring normalMap;
	std::wstring armMap;
	std::wstring emissionMap;
};

struct FBXMeshData
{
	std::wstring meshName;

	FBXMaterialData material;

	std::vector<Vertex> vertex;
	std::vector<unsigned int> indices;
};

struct FBXNode
{
	std::vector<FBXNode> child;
	std::vector<FBXMeshData> meshVec;
};

class FBXLoader
{
public:
	static LazyObjects<FBXLoader> Instance;
	friend LazyObjects<FBXLoader>;

public:
	FBXNode LoadFBXData(const char* filePath);

private:
	void ParseNode(const aiNode* node, const aiScene* scene, FBXNode& fbxNode);
	void ParseMaterial(const aiScene*scene, const aiMesh* mesh, FBXMeshData& meshData);

private:
	std::wstring aiStringToWString(std::string str);
	DirectX::SimpleMath::Matrix ConvertToCloumnMajor(aiMatrix4x4 matrix);

private:
	std::wstring texturePath;
};

