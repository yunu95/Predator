#pragma once
#include "ModelData.h"

#include "Utils.h"

#include <memory>
#include <string>
#include <map>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <vector>

#include <DirectXMath.h>
#include "SimpleMath.h"
using namespace DirectX::PackedVector;

class Mesh;

class ModelLoader
{
public:
	static LazyObjects<ModelLoader> Instance;
	friend LazyObjects<ModelLoader>;

public:
	FBXNode* LoadModel(const char* filePath);

private:
	void ParseNode(const aiNode* node, const aiScene* scene, FBXNode* fbxNode, DirectX::SimpleMath::Matrix parentMatrix);
	void ParseMaterial(const aiScene* scene, const aiMesh* mesh, FBXMeshData& fbxMeshData);
	void AddHasAnimation(FBXNode* fbxNode);
	void LoadAnimation(const aiScene* scene, const std::wstring& fbxName);
	void FillBoneInfoMap(const aiScene* scene);

	void FillVertexBoneIndexAndWeight(const aiScene* scene, const aiNode* node, FBXNode* fbxNode);

	std::wstring aiStringToWString(const aiString& str);
	DirectX::SimpleMath::Matrix ConvertToCloumnMajor(const aiMatrix4x4& matrix);

private:
	std::wstring texturePath;

	std::map<std::wstring, BoneInfo> boneInfoMap;
	std::vector<AnimationClip> animationClipVec;

};

