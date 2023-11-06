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

struct FBXBoneInfo
{
	std::wstring name;
	DirectX::SimpleMath::Matrix offset;
};

struct BoneInfo
{
	std::wstring name;
	int index;
	int parentIndex;
	std::vector<BoneInfo> child;
};


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
	BoneInfo boneInfo;
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
	void ParseMesh(const aiNode* node, const aiScene* scene, FBXNode& fbxNode);
	void ParseMaterial(const aiScene*scene, const aiMesh* mesh, FBXMeshData& meshData);

private:
	std::wstring aiStringToWString(std::string str);
	DirectX::SimpleMath::Matrix ConvertToCloumnMajor(aiMatrix4x4 matrix);
	void BuildBoneHierarchy(const aiNode* node, std::vector<BoneInfo>& boneInfoVec, int parentIndex);

private:
	std::wstring texturePath;
	std::unordered_map<std::wstring,FBXBoneInfo> boneInfoMap;
	static unsigned int currentBoneIndex;
};

