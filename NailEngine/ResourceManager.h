#pragma once

#include "YunuGraphicsInterface.h"

#include "Utils.h"
#include "Struct.h"
#include "ModelData.h"

#include <wrl.h>
#include <memory>
#include <unordered_map>
#include <map>
#include <array>
#include <d3d11.h>


#include "ModelLoader.h"

class Shader;
class Mesh;
class Texture;
class Material;
class Animation;
class AnimationGroup;


class ResourceManager
{
public:
	static LazyObjects<ResourceManager> Instance;
	friend LazyObjects<ResourceManager>;

#pragma region Create
public:
	void CreateDefaultResource();
	void CreateShader(const std::wstring& shaderPath);
	yunuGI::IMaterial* CrateMaterial(std::wstring materialName);
	Material* CreateInstanceMaterial(const Material* material);
	void CreateTexture(const std::wstring& texturePath);
	std::shared_ptr<Texture>& CreateTexture(const std::wstring& texturePath, unsigned int width, unsigned int height, DXGI_FORMAT format, unsigned int bindFlag);
	std::shared_ptr<Texture>& CreateTextureFromResource(const std::wstring& texturePath, Microsoft::WRL::ComPtr<ID3D11Texture2D> tex2D);

	void LoadFBX(const char* filePath);
	void CreateAnimation(const std::vector<AnimationClip>& animationClip, const std::wstring& fbxName);

private:
	void CreateMesh(const std::wstring& mesh);
	void CreateMesh(const std::shared_ptr<Mesh>& mesh);
#pragma endregion

public:
	void* GetFinalRenderImage();
	void PushFBXBoneInfo(const std::wstring fbxName, std::map<std::wstring, BoneInfo>& boneInfoMap);
	void PushFBXNode(const std::wstring fbxName, FBXNode* fbxNode);

#pragma region Getter
	std::shared_ptr<yunuGI::IMaterial> GetMaterial(const std::wstring& materialName);
	std::shared_ptr<yunuGI::IShader> GetShader(const std::wstring& shaderPath);
	std::shared_ptr<yunuGI::IShader> GetShader(const yunuGI::IShader* shader);
	std::shared_ptr<yunuGI::IShader> GetDeferredShader(const std::wstring& shaderPath);
	std::shared_ptr<Mesh> GetMesh(const std::wstring& meshName);
	std::shared_ptr<Texture> GetTexture(const std::wstring& textureName);
	yunuGI::FBXData* GetFBXData(const std::string fbxName);
	///std::vector<FBXBoneInfo>& GetFBXBoneData(const std::string fbxName);
	yunuGI::BoneInfo& GetBoneData(const std::string fbxName);
	std::map<std::wstring, BoneInfo>& GetFBXBoneData(const std::string fbxName);
	FBXNode* GetFBXNode(const std::wstring& fbxName);

	std::vector<yunuGI::IMesh*>& GetMeshList() { return this->meshVec; };
	std::vector<yunuGI::ITexture*>& GetTextureList() { return this->textureVec; };
	std::vector<yunuGI::IMaterial*>& GetMaterialList() 
	{
		return this->materialVec; 
	};
	std::vector<yunuGI::IShader*>& GetShaderList() { return this->shaderVec; };
	std::vector<yunuGI::IAnimation*>& GetAnimationList() { return this->animationVec; };
	std::shared_ptr<AnimationGroup> GetAnimationGroup(const std::wstring& modelName);
#pragma endregion

private:
	void CreateDeferredShader(const std::wstring& shaderPath);
	void CreateDefaultShader();
	void CreateDefaultMesh();
	void CreateDefaultMaterial();
	void CreateDefaultTexture();

	void FillFBXData(const std::wstring& fbxName,FBXNode* node, yunuGI::FBXData* fbxData);
	void FillFBXBoneInfoVec(const yunuGI::BoneInfo& boneInfo, std::vector<yunuGI::BoneInfo>& boneInfoVec);
	//void CreateResourceFromFBX(FBXMeshData& meshData, std::vector<yunuGI::FBXData>& dataVec, yunuGI::FBXData& fbxData);
	

#pragma region LoadMesh
	void LoadCubeMesh();
	void LoadSphereMesh();
	void LoadRactangleMesh();
	void LoadPointMesh();
	void LoadLineMesh();
	void LoadCapsuleMesh();
	void LoadCylinderMesh();
#pragma endregion



private:
	// Mesh 관련
	std::vector<yunuGI::IMesh*> meshVec;
	std::unordered_map<std::wstring, std::shared_ptr<yunuGI::IMesh>> meshMap;

	// Texture 관련
	std::vector<yunuGI::ITexture*> textureVec;
	std::unordered_map<std::wstring, std::shared_ptr<yunuGI::ITexture>> textureMap;
	std::unordered_map<std::wstring, std::shared_ptr<yunuGI::ITexture>> deferredTextureMap;

	// Material 관련
	std::vector<yunuGI::IMaterial*> materialVec;
	std::unordered_map<std::wstring, std::shared_ptr<yunuGI::IMaterial>> materialMap;
	// Graphics 내부에서 사용 할 Material
	std::unordered_map<std::wstring, std::shared_ptr<yunuGI::IMaterial>> instanceMaterialMap;
	std::unordered_map<std::wstring, std::shared_ptr<yunuGI::IMaterial>> deferredMaterialMap;

	// Shader 관련
	std::vector<yunuGI::IShader*> shaderVec;
	std::unordered_map<std::wstring, std::shared_ptr<yunuGI::IShader>> shaderMap;
	std::unordered_map<std::wstring, std::shared_ptr<yunuGI::IShader>> deferredShaderMap;

	// Animation 관련
	std::vector<yunuGI::IAnimation*> animationVec;
	std::unordered_map<std::wstring, std::shared_ptr<yunuGI::IAnimation>> animationMap;
	// 그래픽스에서 애니메이션을 텍스처로 활용하고 싶어서 만든 컨테이너
	std::unordered_map<std::wstring, std::shared_ptr<AnimationGroup>> animationGroupMap;

	std::unordered_map<std::wstring, yunuGI::FBXData*> fbxDataMap;
	//// 게임 엔진에서 본 계층구조로 오브젝트 만들 때 쓰는용
	///std::unordered_map<std::wstring, std::vector<yunuGI::BoneInfo>> fbxBoneInfoVecMap;
	// 그래픽스 엔진 내부에서 스키닝 애니메이션에 쓸 오프셋 행렬을 가지고 있는 본 정보
	std::unordered_map<std::wstring, std::map<std::wstring,BoneInfo>> fbxBoneInfoMap;
	std::unordered_map<std::wstring, FBXNode*> fbxNodeMap;

	//
	ID3D11ShaderResourceView* renderImageView = nullptr;
	//std::unordered_map<std::wstring, std::shared_ptr<Shader>> shaderMap;
	//std::unordered_map<std::wstring, std::shared_ptr<Texture>> textureMap;
	//std::unordered_map<std::wstring, std::shared_ptr<Mesh>> meshMap;
	//std::unordered_map<std::wstring, std::shared_ptr<yunuGIAdapter::MaterialAdapter>> materialMap;
	//std::unordered_map<std::wstring, std::shared_ptr<yunuGIAdapter::MaterialAdapter>> instanceMaterialMap;
	
	//std::unordered_map<std::wstring, yunuGI::BoneInfo> fbxBoneInfoTreeMap;

	//// 그래픽스에서 스키닝할 때 쓰는 offset matrix를 가지고 있는 본 정보
	//std::unordered_map<std::wstring, std::vector<FBXBoneInfo>> fbxBonOffsetVecMap;
};

