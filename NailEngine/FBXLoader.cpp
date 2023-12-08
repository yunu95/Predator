#include "FBXLoader.h"

#include <iostream>
#include <codecvt>
#include <locale>
#include <filesystem>

#include "ResourceManager.h"
#include "Mesh.h"

LazyObjects<FBXLoader> FBXLoader::Instance;
unsigned int FBXLoader::currentBoneIndex = 1;


FBXNode FBXLoader::LoadFBXData(const char* filePath)
{
	this->texturePath = std::filesystem::path(filePath).parent_path().wstring()
		+ L"/" + std::filesystem::path(filePath).stem().wstring() + L".fbm/";

	// Assimp Importer 객체 생성
	Assimp::Importer importer;

	// Load Flag
	unsigned int flag;
	//flag = aiProcess_Triangulate |
	//	aiProcess_JoinIdenticalVertices |
	//	aiProcess_CalcTangentSpace |
	//	aiProcess_GenSmoothNormals |
	//	aiProcess_FlipWindingOrder |
	//	aiProcess_FlipUVs |
	//	aiProcess_SplitLargeMeshes |
	//	aiProcess_PreTransformVertices |
	//	aiProcess_ConvertToLeftHanded |
	//	aiProcess_PopulateArmatureData |
	//	aiProcess_LimitBoneWeights |
	//	aiProcess_SortByPType |
	//	aiProcess_GenBoundingBoxes |
	//	aiProcess_EmbedTextures;

	flag = aiProcess_Triangulate |
		aiProcess_ConvertToLeftHanded | aiProcess_JoinIdenticalVertices | aiProcess_GenBoundingBoxes |
		aiProcess_CalcTangentSpace | aiProcess_PopulateArmatureData |
		aiProcess_GenSmoothNormals | aiProcess_SplitLargeMeshes |
		aiProcess_SortByPType | aiProcess_LimitBoneWeights;

	//flag = aiProcess_Triangulate |
	//	aiProcess_ConvertToLeftHanded | aiProcess_JoinIdenticalVertices | aiProcess_GenBoundingBoxes |
	//	aiProcess_CalcTangentSpace | aiProcess_PopulateArmatureData |
	//	aiProcess_FlipWindingOrder | aiProcess_GenSmoothNormals | aiProcess_SplitLargeMeshes |
	//	aiProcess_SortByPType | aiProcess_LimitBoneWeights;

	const aiScene* scene = importer.ReadFile(filePath, flag);

	if (!scene) {
		std::cerr << "모델 로딩 실패!" << std::endl;
	}

	// Mesh Data Load
	FBXNode rootNode;
	ParseNode(scene->mRootNode, scene, rootNode);

	// Animation Load
	//ParseAnimation();

	yunuGI::BoneInfo boneInfo;
	boneInfo.index = 0;
	boneInfo.parentIndex = -1;
	boneInfo.name = L"BoneRoot";

	// Load한 Bone을 계층 구조로 만들기
	BuildBoneHierarchy(scene->mRootNode, boneInfo.child, 0);
	rootNode.boneInfo = std::move(boneInfo);

	if (scene->mNumAnimations > 0)
	{
		rootNode.hasAnimation = true;
		LoadAnimation(scene);
	}

	boneInfoMap.clear();
	currentBoneIndex = 1;
	texturePath.clear();

	return rootNode;
}

void FBXLoader::ParseNode(const aiNode* node, const aiScene* scene, FBXNode& fbxNode)
{
	// Mesh Load And Material Load
	ParseMesh(node, scene, fbxNode);

	// Parse Child
	fbxNode.child.resize(node->mNumChildren);
	for (unsigned int i = 0; i < node->mNumChildren; ++i)
	{
		ParseNode(node->mChildren[i], scene, fbxNode.child[i]);
	}
}

void FBXLoader::ParseMesh(const aiNode* node, const aiScene* scene, FBXNode& fbxNode)
{
	std::wstring asd = this->aiStringToWString(node->mName.C_Str());

	for (unsigned int i = 0; i < node->mNumMeshes; ++i)
	{
		FBXMeshData data;

		unsigned int meshIndex = node->mMeshes[i];
		const aiMesh* mesh = scene->mMeshes[meshIndex];

		std::wstring meshName = aiStringToWString(mesh->mName.C_Str());

		data.meshName = meshName;
		aiMatrix4x4 transform = node->mTransformation;

		for (int j = 0; j < mesh->mNumVertices; ++j)
		{
			// 버텍스 위치 (Position)
			aiVector3D vertexPos = mesh->mVertices[j];
			vertexPos *= transform;
			DirectX::SimpleMath::Vector3 dvertex{ vertexPos.x,vertexPos.y,vertexPos.z };

			// 버텍스 색상 (Color) - 대개 사용되지 않음
			///aiColor4D color = mesh->mColors[0][i]; // 첫 번째 색상 채널

			// UV 좌표 (Texture Coordinates)
			aiVector3D uv = mesh->mTextureCoords[0][j]; // 첫 번째 UV 채널
			DirectX::SimpleMath::Vector2 duv{ uv.x, uv.y };

			// 법선 (Normal)
			aiVector3D normal = mesh->mNormals[j];
			normal *= transform;
			DirectX::SimpleMath::Vector3 dnormal{ normal.x, normal.y, normal.z };

			// 탄젠트 (Tangent)
			///aiVector3D tangent = mesh->mTangents[i];

			Vertex vertex = { dvertex , DirectX::SimpleMath::Vector4(1.f,1.f,1.f,1.f), duv, dnormal, DirectX::SimpleMath::Vector3(1.f,0.f,0.f) };

			data.vertex.emplace_back(vertex);
		}

		/// 인덱스 채우기
		for (int j = 0; j < mesh->mNumFaces; ++j)
		{
			aiFace& face = mesh->mFaces[j];
			for (int k = 0; k < face.mNumIndices; ++k)
			{
				data.indices.emplace_back(face.mIndices[k]);
			}
		}

		// Material Load
		ParseMaterial(scene, mesh, data);

		// Vertex에 Bone 정보 넣기
		for (int b = 0; b < mesh->mNumBones; ++b)
		{
			for (int w = 0; w < mesh->mBones[b]->mNumWeights; ++w)
			{
				FBXBoneInfo boneInfo;
				boneInfo.name = this->aiStringToWString(mesh->mBones[b]->mName.C_Str());
				boneInfo.offset = this->ConvertToCloumnMajor(mesh->mBones[b]->mOffsetMatrix);

				this->boneInfoMap.insert({ boneInfo.name,boneInfo });

				data.vertex[mesh->mBones[b]->mWeights[w].mVertexId].FillBoneIndexWeight(b, mesh->mBones[b]->mWeights[w].mWeight);
			}
		}


		fbxNode.meshVec.emplace_back(data);
	}
}

void FBXLoader::ParseMaterial(const aiScene* scene, const aiMesh* mesh, FBXMeshData& meshData)
{
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

	// Material Name
	meshData.material.materialName = aiStringToWString(material->GetName().C_Str());

	// Albedo Texture
	if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
	{
		aiString path;
		if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS)
		{
			std::wstring _path = aiStringToWString(path.C_Str());
			std::filesystem::path pathName(_path);
			pathName.filename().wstring();

			meshData.material.albedoMap = this->texturePath + pathName.filename().wstring();
			//meshData.material.albedoMap = this->texturePath + L"T_Spear_D.png";
		}
	}

	// Normal Texture
	if (material->GetTextureCount(aiTextureType_NORMALS) > 0)
	{
		aiString path;
		if (material->GetTexture(aiTextureType_NORMALS, 0, &path) == AI_SUCCESS)
		{
			std::wstring _path = aiStringToWString(path.C_Str());
			std::filesystem::path pathName(_path);
			pathName.filename().wstring();

			meshData.material.normalMap = this->texturePath + pathName.filename().wstring();
			//meshData.material.albedoMap = this->texturePath + L"T_Spear_N.png";
		}
	}
}

void FBXLoader::LoadAnimation(const aiScene* scene)
{
	for (int i = 0; i < scene->mNumAnimations; ++i)
	{
		aiAnimation* animation = scene->mAnimations[i];

		AnimationClip animationClip;
		animationClip.name = this->aiStringToWString(animation->mName.C_Str());
		animationClip.duration = animation->mDuration / animation->mTicksPerSecond;
		animationClip.totlaFrame = animation->mDuration;
		int totalFrame = animation->mDuration;

		animationClip.keyFrameInfoVec.resize(this->boneInfoMap.size() + 1);

		for (int j = 0; j < animation->mNumChannels; ++j)
		{
			aiNodeAnim* nodeAnim = animation->mChannels[j];
			nodeAnim->mNodeName;

			auto iter = this->boneInfoMap.find(this->aiStringToWString(nodeAnim->mNodeName.C_Str()));

			if (iter != this->boneInfoMap.end())
			{
				KeyFrameInfo keyFrameInfo;
				keyFrameInfo.srtVec.resize(animationClip.totlaFrame + 1 , DirectX::SimpleMath::Matrix::Identity);

				std::vector<DirectX::SimpleMath::Matrix> keyPosMatrix;
				keyPosMatrix.resize(animationClip.totlaFrame + 1);

				std::vector<DirectX::SimpleMath::Matrix> keyRotMatrix;
				keyRotMatrix.resize(animationClip.totlaFrame + 1);

				std::vector<DirectX::SimpleMath::Matrix> keyScaleMatrix;
				keyScaleMatrix.resize(animationClip.totlaFrame + 1);

				for (int k = 0; k < nodeAnim->mNumPositionKeys; ++k)
				{
					aiVectorKey vectorKey = nodeAnim->mPositionKeys[k];
					keyPosMatrix[static_cast<int>(vectorKey.mTime)] =
						DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3{ vectorKey.mValue.x,vectorKey.mValue.y,vectorKey.mValue.z });
				}
				for (int k = 0; k < nodeAnim->mNumRotationKeys; ++k)
				{
					aiQuatKey quatKey = nodeAnim->mRotationKeys[k];
					keyRotMatrix[static_cast<int>(quatKey.mTime)] =
						DirectX::XMMatrixRotationQuaternion(DirectX::SimpleMath::Vector4{ quatKey.mValue.x,quatKey.mValue.y,quatKey.mValue.z, quatKey.mValue.w });
				}
				for (int k = 0; k < nodeAnim->mNumScalingKeys; ++k)
				{
					aiVectorKey vectorKey = nodeAnim->mScalingKeys[k];
					keyScaleMatrix[static_cast<int>(vectorKey.mTime)] =
						DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3{ vectorKey.mValue.x,vectorKey.mValue.y,vectorKey.mValue.z });
				}

				for (int k = 0; k < keyPosMatrix.size(); ++k)
				{
					DirectX::SimpleMath::Matrix srt = DirectX::SimpleMath::Matrix::Identity;
					srt = keyScaleMatrix[k] * keyRotMatrix[k] * keyPosMatrix[k];
					keyFrameInfo.srtVec[k] = (srt);
				}
				
				animationClip.keyFrameInfoVec[iter->second.index] = std::move(keyFrameInfo);
			}
		}

		ResourceManager::Instance.Get().CreateAnimation(animationClip);
	}
}

std::wstring FBXLoader::aiStringToWString(std::string str)
{
	std::wstring wstr;
	for (char c : str) {
		wstr += static_cast<wchar_t>(c);
	}
	return wstr;
}

DirectX::SimpleMath::Matrix FBXLoader::ConvertToCloumnMajor(aiMatrix4x4 matrix)
{
	DirectX::SimpleMath::Matrix colMajor{
		matrix.a1, matrix.b1, matrix.c1,matrix.d1,
		matrix.a2, matrix.b2, matrix.c2,matrix.d2,
		matrix.a3, matrix.b3, matrix.c3,matrix.d3,
		matrix.a4, matrix.b4, matrix.c4,matrix.d4,
	};

	return colMajor;
}

void FBXLoader::BuildBoneHierarchy(const aiNode* node, std::vector<yunuGI::BoneInfo>& boneInfoVec, int parentIndex)
{
	auto iter = this->boneInfoMap.find(this->aiStringToWString(node->mName.C_Str()));
	if (iter != this->boneInfoMap.end())
	{
		yunuGI::BoneInfo boneInfo;

		boneInfo.name = iter->second.name;

		boneInfo.index = currentBoneIndex++;
		iter->second.index = boneInfo.index;

		boneInfo.parentIndex = parentIndex;
		iter->second.parentIndex = boneInfo.parentIndex;

		boneInfoVec.emplace_back(boneInfo);

		for (int i = 0; i < node->mNumChildren; ++i)
		{
			BuildBoneHierarchy(node->mChildren[i], boneInfoVec.back().child, boneInfo.index);
		}
	}
	else
	{
		for (int i = 0; i < node->mNumChildren; ++i)
		{
			BuildBoneHierarchy(node->mChildren[i], boneInfoVec, parentIndex);
		}
	}
}
