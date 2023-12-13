#pragma once
#include "FBXData.h"

#include "Utils.h"
#include "Struct.h"
#include "Animation.h"

#include <memory>
#include <string>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <vector>
#include <map>

#include <DirectXMath.h>
#include "SimpleMath.h"
using namespace DirectX::PackedVector;

class Mesh;

struct FBXBoneInfo
{
	std::wstring name;
	int parentIndex;
	int index;
	DirectX::SimpleMath::Matrix offset;
	DirectX::SimpleMath::Matrix localTM;
};

//struct BoneInfo
//{
//	std::wstring name;
//	int index;
//	int parentIndex;
//	std::vector<BoneInfo> child;
//};

//struct FBXMaterialData
//{
//	std::wstring materialName;
//	std::wstring albedoMap;
//	std::wstring normalMap;
//	std::wstring armMap;
//	std::wstring emissionMap;
//};

struct FBXMeshData
{
	std::wstring meshName;

	yunuGI::MaterialData material;

	std::vector<Vertex> vertex;
	std::vector<unsigned int> indices;
};

struct FBXNode
{
	std::vector<FBXNode> child;
	std::vector<FBXMeshData> meshVec;
	yunuGI::BoneInfo boneInfo;
	std::wstring nodeName;
	DirectX::SimpleMath::Matrix transformMatrix;
	bool hasAnimation;
};

class FBXLoader
{
public:
	static LazyObjects<FBXLoader> Instance;
	friend LazyObjects<FBXLoader>;

public:
	FBXNode LoadFBXData(const char* filePath);

private:
	void ParseNode(const aiNode* rootNode, const aiNode* node, const aiScene* scene, FBXNode& fbxNode, FBXNode& fbxRootNode);
	void ParseMesh(const aiNode* rootNode, const aiNode* node, const aiScene* scene, FBXNode& fbxNode, FBXNode& fbxRootNode);
	void ParseMaterial(const aiScene*scene, const aiMesh* mesh, FBXMeshData& meshData);
	void LoadAnimation(const aiScene* scene);

private:
	std::wstring aiStringToWString(std::string str);
	DirectX::SimpleMath::Matrix ConvertToCloumnMajor(aiMatrix4x4 matrix);
	void BuildBoneHierarchy(const aiNode* node, std::vector<yunuGI::BoneInfo>& boneInfoVec, int parentIndex);

	void FindRoot(const aiNode* node);

private:
	std::wstring texturePath;
	std::unordered_map<std::wstring, FBXBoneInfo> boneInfoMap;

	std::vector<FBXBoneInfo> fbxBoneInfoVec;

	static unsigned int currentBoneIndex;

	bool isFirst = true;
};

