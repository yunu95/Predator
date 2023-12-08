#pragma once
#include "FBXData.h"

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
	int parentIndex;
	int index;
	DirectX::SimpleMath::Matrix offset;
};

struct KeyFrameInfo
{
	std::wstring boneName;
	std::vector<DirectX::SimpleMath::Matrix> srtVec;
};

struct AnimationClip
{
	std::wstring name;
	double duration;
	int totlaFrame;
	std::vector<KeyFrameInfo> keyFrameInfoVec;
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
	void ParseNode(const aiNode* node, const aiScene* scene, FBXNode& fbxNode);
	void ParseMesh(const aiNode* node, const aiScene* scene, FBXNode& fbxNode);
	void ParseMaterial(const aiScene*scene, const aiMesh* mesh, FBXMeshData& meshData);
	void LoadAnimation(const aiScene* scene);

private:
	std::wstring aiStringToWString(std::string str);
	DirectX::SimpleMath::Matrix ConvertToCloumnMajor(aiMatrix4x4 matrix);
	void BuildBoneHierarchy(const aiNode* node, std::vector<yunuGI::BoneInfo>& boneInfoVec, int parentIndex);

private:
	std::wstring texturePath;
	std::unordered_map<std::wstring, FBXBoneInfo> boneInfoMap;
	static unsigned int currentBoneIndex;
};

