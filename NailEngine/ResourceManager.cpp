#include "ResourceManager.h"

#include <assert.h>
#include <filesystem>

#include "SimpleMath.h"
using namespace DirectX::PackedVector;

#include "VertexShader.h"
#include "PixelShader.h"
#include "ResourceBuilder.h"
#include "Device.h"
#include "Mesh.h"
#include "IMaterial.h"
#include "Material.h"
#include "Texture.h"

#include "NailEngine.h"
#include "RenderTargetGroup.h"


LazyObjects<ResourceManager> ResourceManager::Instance;

void ResourceManager::CreateDefaultResource()
{
	CreateDefaultShader();
	CreateDefaultMesh();
	CreateDefaultTexture();
	CreateDefaultMaterial();
}

void ResourceManager::CreateShader(const std::wstring& shaderPath)
{
	size_t dotPos = shaderPath.find_last_of(L".");

	std::wstring shaderType = shaderPath.substr(dotPos - 2, 2);
	std::shared_ptr<yunuGI::IShader> shader = nullptr;

	if (shaderType == L"VS")
	{
		shader = std::make_shared<VertexShader>();
	}
	else if (shaderType == L"PS")
	{
		shader = std::make_shared<PixelShader>();
	}

	if (shader != nullptr)
	{
		std::static_pointer_cast<Shader>(shader)->CreateShader(shaderPath);
	}

	std::filesystem::path _shaderName(shaderPath);
	std::wstring shaderName = _shaderName.filename().wstring();

	shader->SetName(shaderName);

	shaderVec.emplace_back(shader.get());
	shaderMap.insert({ shaderName , std::move(shader) });
}

void ResourceManager::CreateMesh(const std::wstring& mesh)
{
	if (mesh == L"Cube")
	{
		LoadCubeMesh();
		return;
	}
	else if (mesh == L"Sphere")
	{
		LoadSphereMesh();
		return;
	}
	else if (mesh == L"Rectangle")
	{
		LoadRactangleMesh();
		return;
	}
	else if (mesh == L"Point")
	{
		LoadPointMesh();
		return;
	}
	else if (mesh == L"Line")
	{
		LoadLineMesh();
		return;
	}
	else if (mesh == L"Capsule")
	{
		LoadCapsuleMesh();
		return;
	}
	else if (mesh == L"Cylinder")
	{
		LoadCylinderMesh();
		return;
	}
}

void ResourceManager::CreateMesh(const std::shared_ptr<Mesh>& mesh)
{
	meshVec.emplace_back(mesh.get());
	meshMap.insert({ mesh->GetName(), mesh });
}

yunuGI::IMaterial* ResourceManager::CrateMaterial(std::wstring materialName)
{
	std::shared_ptr<Material> material = std::make_shared<Material>();

	if (materialName.empty())
	{
		materialName = L"DefaultMaterial";
	}

	material->SetName(materialName);

	materialMap.insert({ materialName, material });

	return material.get();
}

std::shared_ptr<Material> ResourceManager::CreateInstanceMaterial(const std::shared_ptr<Material> material)
{
	Material* _material = new Material(*material);
	std::shared_ptr<Material> instanceMaterial(_material);

	std::wstring materialName = instanceMaterial->GetName();
	materialName += L"_instance_";
	materialName += std::to_wstring(instanceMaterial->GetID());
	instanceMaterial->SetName(materialName);


	instanceMaterialMap.insert({ materialName, instanceMaterial });

	return instanceMaterial;
}

void ResourceManager::CreateTexture(const std::wstring& texturePath)
{
	std::shared_ptr<Texture> texture = std::make_shared<Texture>();

	texture->LoadTexture(texturePath);
	texture->SetName(texturePath);

	textureMap.insert({ texturePath, texture });
}

std::shared_ptr<Texture>& ResourceManager::CreateTexture(const std::wstring& texturePath, unsigned int width, unsigned int height, DXGI_FORMAT format, unsigned int bindFlag)
{
	std::shared_ptr<Texture> texture = std::make_shared<Texture>();

	texture->CreateTexture(texturePath, width, height, format, bindFlag);
	texture->SetName(texturePath);

	this->deferredTextureMap.insert({ texturePath, texture });
	//this->textureMap.insert({ texturePath, texture });

	return texture;
}

std::shared_ptr<Texture>& ResourceManager::CreateTextureFromResource(const std::wstring& texturePath, Microsoft::WRL::ComPtr<ID3D11Texture2D> tex2D)
{
	std::shared_ptr<Texture> texture = std::make_shared<Texture>();

	texture->CreateFromResource(tex2D);
	texture->SetName(texturePath);

	textureMap.insert({ texturePath, texture });

	return texture;
}

void ResourceManager::LoadFBX(const char* filePath)
{
	FBXNode node = FBXLoader::Instance.Get().LoadFBXData(filePath);

	std::vector<yunuGI::FBXData> dataVec;
	std::wstring fbxName = std::filesystem::path(filePath).stem().wstring();

	this->fbxDataVecMap.insert({ fbxName, dataVec });

	//this->fbxBoneInfoTreeMap.insert({ fbxName,node.boneInfo });

	FillFBXData(fbxName, node, this->fbxDataVecMap.find(fbxName)->second);

	std::vector<yunuGI::BoneInfo> boneInfoVec;
	this->fbxBoneInfoVecMap.insert({ fbxName, std::move(boneInfoVec) });
	auto iter = this->fbxBoneInfoVecMap.find(fbxName);
	FillFBXBoneInfoVec(node.boneInfo, iter->second);
}

std::shared_ptr<yunuGI::IMaterial> ResourceManager::GetMaterial(const std::wstring& materialName)
{
	auto iter = materialMap.find(materialName);
	if (iter == materialMap.end())
	{
		auto iter2 = instanceMaterialMap.find(materialName);
		if (iter2 == instanceMaterialMap.end())
		{
			return nullptr;
		}
		else
		{
			return iter2->second;
		}
	}
	else
	{
		return iter->second;
	}
}

std::shared_ptr<yunuGI::IShader> ResourceManager::GetShader(const std::wstring& shaderPath)
{
	auto iter = shaderMap.find(shaderPath);
	assert(iter != shaderMap.end());

	return std::static_pointer_cast<Shader>(iter->second);
}

std::shared_ptr<yunuGI::IShader> ResourceManager::GetShader(const yunuGI::IShader* shader)
{
	auto iter = this->shaderMap.find(shader->GetName());
	if (iter != this->shaderMap.end())
	{
		return iter->second;
	}

	iter = this->deferredShaderMap.find(shader->GetName());
	if (iter != this->deferredShaderMap.end())
	{
		return iter->second;
	}

	return nullptr;
}

std::shared_ptr<yunuGI::IShader> ResourceManager::GetDeferredShader(const std::wstring& shaderPath)
{
	auto iter = deferredShaderMap.find(shaderPath);
	assert(iter != deferredShaderMap.end());

	return std::static_pointer_cast<Shader>(iter->second);
}

std::shared_ptr<Mesh> ResourceManager::GetMesh(const std::wstring& meshName)
{
	auto iter = meshMap.find(meshName);
	assert(iter != meshMap.end());

	return std::static_pointer_cast<Mesh>(iter->second);
}

std::shared_ptr<Texture> ResourceManager::GetTexture(const std::wstring& textureName)
{
	auto iter = textureMap.find(textureName);
	if (iter != textureMap.end())
	{
		return std::static_pointer_cast<Texture>(iter->second);
	}
	iter = deferredTextureMap.find(textureName);
	if (iter != deferredTextureMap.end())
	{
		return std::static_pointer_cast<Texture>(iter->second);
	}
	return nullptr;
}

std::vector<yunuGI::FBXData>& ResourceManager::GetFBXData(const std::string fbxName)
{
	std::wstring fbxNameW = std::wstring{ fbxName.begin(), fbxName.end() };

	auto iter = this->fbxDataVecMap.find(fbxNameW);
	assert(iter != this->fbxDataVecMap.end());
	return iter->second;
}

yunuGI::BoneInfo& ResourceManager::GetFBXBoneData(const std::string fbxName)
{
	//std::wstring fbxNameW = std::wstring{ fbxName.begin(), fbxName.end() };

	//auto iter = this->fbxBoneInfoTreeMap.find(fbxNameW);
	//assert(iter != this->fbxBoneInfoTreeMap.end());

	//return iter->second;
	yunuGI::BoneInfo a{};
	return a;
}

void ResourceManager::CreateDeferredShader(const std::wstring& shaderPath)
{
	size_t dotPos = shaderPath.find_last_of(L".");

	std::wstring shaderType = shaderPath.substr(dotPos - 2, 2);
	std::shared_ptr<yunuGI::IShader> shader = nullptr;

	if (shaderType == L"VS")
	{
		shader = std::make_shared<VertexShader>();
	}
	else if (shaderType == L"PS")
	{
		shader = std::make_shared<PixelShader>();
	}

	if (shader != nullptr)
	{
		std::static_pointer_cast<Shader>(shader)->CreateShader(shaderPath);
	}

	std::filesystem::path _shaderName(shaderPath);
	std::wstring shaderName = _shaderName.filename().wstring();

	shader->SetName(shaderName);

	deferredShaderMap.insert({ shaderName , std::move(shader) });
}

void ResourceManager::CreateDefaultShader()
{
#pragma region VS
	CreateShader(L"DefaultVS.cso");
	CreateDeferredShader(L"Deferred_DirectionalLightVS.cso");
	CreateDeferredShader(L"Deferred_PointLightVS.cso");
	CreateDeferredShader(L"Deferred_FinalVS.cso");
	CreateShader(L"TextureVS.cso");
#pragma endregion

#pragma region PS
	CreateShader(L"DefaultPS.cso");
	CreateShader(L"DebugPS.cso");
	CreateDeferredShader(L"Deferred_DirectionalLightPS.cso");
	CreateDeferredShader(L"Deferred_PointLightPS.cso");
	CreateDeferredShader(L"Deferred_FinalPS.cso");
	CreateShader(L"TexturePS.cso");
#pragma endregion
}

void ResourceManager::CreateDefaultMesh()
{
	CreateMesh(L"Cube");
	CreateMesh(L"Sphere");
	CreateMesh(L"Rectangle");
	/*CreateMesh(L"Point");
	CreateMesh(L"Line");*/
	CreateMesh(L"Capsule");
	CreateMesh(L"Cylinder");
}

void ResourceManager::CreateDefaultMaterial()
{
	// DefaultMaterial
	{
		std::wstring name{};
		CrateMaterial(name);
	}

	auto& renderTargetGroupVec = NailEngine::Instance.Get().GetRenderTargetGroup();

	// DirectionalLight
	{
		yunuGI::IMaterial* material = CrateMaterial(L"Deferred_DirectionalLight");
		material->SetVertexShader(GetDeferredShader(L"Deferred_DirectionalLightVS.cso").get());
		material->SetPixelShader(GetDeferredShader(L"Deferred_DirectionalLightPS.cso").get());
		material->SetTexture(yunuGI::Texture_Type::Temp0, 
			renderTargetGroupVec[static_cast<int>(RENDER_TARGET_TYPE::G_BUFFER)]->GetRTTexture(static_cast<int>(POSITION)).get());
		material->SetTexture(yunuGI::Texture_Type::Temp1,
			renderTargetGroupVec[static_cast<int>(RENDER_TARGET_TYPE::G_BUFFER)]->GetRTTexture(static_cast<int>(NORMAL)).get());
	}

	// PointLight
	{
		yunuGI::IMaterial* material = CrateMaterial(L"Deferred_PointLight");
		material->SetVertexShader(GetDeferredShader(L"Deferred_PointLightVS.cso").get());
		material->SetPixelShader(GetDeferredShader(L"Deferred_PointLightPS.cso").get());
		material->SetTexture(yunuGI::Texture_Type::Temp0,
			renderTargetGroupVec[static_cast<int>(RENDER_TARGET_TYPE::G_BUFFER)]->GetRTTexture(static_cast<int>(POSITION)).get());
		material->SetTexture(yunuGI::Texture_Type::Temp1,
			renderTargetGroupVec[static_cast<int>(RENDER_TARGET_TYPE::G_BUFFER)]->GetRTTexture(static_cast<int>(NORMAL)).get());
	}

	// Deferred_Final
	{
		yunuGI::IMaterial* material = CrateMaterial(L"Deferred_Final");
		material->SetVertexShader(GetDeferredShader(L"Deferred_FinalVS.cso").get());
		material->SetPixelShader(GetDeferredShader(L"Deferred_FinalPS.cso").get());
		material->SetTexture(yunuGI::Texture_Type::Temp0, 
			renderTargetGroupVec[static_cast<int>(RENDER_TARGET_TYPE::G_BUFFER)]->GetRTTexture(static_cast<int>(ALBEDO)).get());
		material->SetTexture(yunuGI::Texture_Type::Temp1, 
			renderTargetGroupVec[static_cast<int>(RENDER_TARGET_TYPE::LIGHTING)]->GetRTTexture(static_cast<int>(DIFFUSE)).get());
		material->SetTexture(yunuGI::Texture_Type::Temp2, 
			renderTargetGroupVec[static_cast<int>(RENDER_TARGET_TYPE::LIGHTING)]->GetRTTexture(static_cast<int>(SPECULAR)).get());
	}

	/// Deferred Debug Info
	{
		{
			// Position
			yunuGI::IMaterial* material = CrateMaterial(L"DeferredPosition");
			material->SetPixelShader(GetShader(L"TexturePS.cso").get());
			material->SetVertexShader(GetShader(L"TextureVS.cso").get());
			material->SetTexture(yunuGI::Texture_Type::Temp0,
				renderTargetGroupVec[static_cast<int>(RENDER_TARGET_TYPE::G_BUFFER)]->GetRTTexture(static_cast<int>(POSITION)).get());
		}

		{
			// Normal
			yunuGI::IMaterial* material = CrateMaterial(L"DeferredNormal");
			material->SetPixelShader(GetShader(L"TexturePS.cso").get());
			material->SetVertexShader(GetShader(L"TextureVS.cso").get());
			material->SetTexture(yunuGI::Texture_Type::Temp0,
				renderTargetGroupVec[static_cast<int>(RENDER_TARGET_TYPE::G_BUFFER)]->GetRTTexture(static_cast<int>(NORMAL)).get());
		}

		{
			// Color
			yunuGI::IMaterial* material = CrateMaterial(L"DeferredColor");
			material->SetPixelShader(GetShader(L"TexturePS.cso").get());
			material->SetVertexShader(GetShader(L"TextureVS.cso").get());
			material->SetTexture(yunuGI::Texture_Type::Temp0, 
				renderTargetGroupVec[static_cast<int>(RENDER_TARGET_TYPE::G_BUFFER)]->GetRTTexture(static_cast<int>(ALBEDO)).get());
		}

		{
			// Depth
			yunuGI::IMaterial* material = CrateMaterial(L"DeferredDepth");
			material->SetPixelShader(GetShader(L"TexturePS.cso").get());
			material->SetVertexShader(GetShader(L"TextureVS.cso").get());
			material->SetTexture(yunuGI::Texture_Type::Temp0,
				renderTargetGroupVec[static_cast<int>(RENDER_TARGET_TYPE::G_BUFFER)]->GetRTTexture(static_cast<int>(DEPTH)).get());
		}

		{
			// DiffuseLight
			yunuGI::IMaterial* material = CrateMaterial(L"DeferredDiffuseLight");
			material->SetPixelShader(GetShader(L"TexturePS.cso").get());
			material->SetVertexShader(GetShader(L"TextureVS.cso").get());
			material->SetTexture(yunuGI::Texture_Type::Temp0, 
				renderTargetGroupVec[static_cast<int>(RENDER_TARGET_TYPE::LIGHTING)]->GetRTTexture(static_cast<int>(DIFFUSE)).get());
		}

		{
			// SpecularLight
			yunuGI::IMaterial* material = CrateMaterial(L"DeferredSpecularLight");
			material->SetPixelShader(GetShader(L"TexturePS.cso").get());
			material->SetVertexShader(GetShader(L"TextureVS.cso").get());
			material->SetTexture(yunuGI::Texture_Type::Temp0, 
				renderTargetGroupVec[static_cast<int>(RENDER_TARGET_TYPE::LIGHTING)]->GetRTTexture(static_cast<int>(SPECULAR)).get());
		}
	}
}

void ResourceManager::CreateDefaultTexture()
{
	CreateTexture(L"Texture/zoro.jpg");
	CreateTexture(L"Texture/Brick_Albedo.jpg");
	CreateTexture(L"Texture/Brick_Normal.jpg");
}

void ResourceManager::FillFBXData(const std::wstring& fbxName, FBXNode& node, std::vector<yunuGI::FBXData>& dataVec)
{
	for (int i = 0; i < node.meshVec.size(); ++i)
	{
		auto iter = this->meshMap.find(node.meshVec[i].meshName);

		// 이미 메쉬 데이터가 있다면 이 메쉬는 다중 머터리얼임
		if (iter != this->meshMap.end())
		{
			std::static_pointer_cast<Mesh>(iter->second)->SetData(node.meshVec[i].vertex, node.meshVec[i].indices);

			for (auto& e : this->fbxDataVecMap.find(fbxName)->second)
			{
				if (e.meshName != node.meshVec[i].meshName) continue;

				CreateResourceFromFBX(node.meshVec[i], this->fbxDataVecMap.find(fbxName)->second, e);
			}
		}
		else
		{
			// 메쉬 데이터가 없다면 메쉬를 생성 후 삽입
			std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
			mesh->SetName(node.meshVec[i].meshName);
			mesh->SetData(node.meshVec[i].vertex, node.meshVec[i].indices);
			CreateMesh(mesh);

			yunuGI::FBXData data;
			CreateResourceFromFBX(node.meshVec[i], dataVec,  data);
		}
	}

	for (int i = 0; i < node.child.size(); ++i)
	{
		FillFBXData(fbxName, node.child[i], dataVec);
	}
}

void ResourceManager::FillFBXBoneInfoVec(const yunuGI::BoneInfo& boneInfo, std::vector<yunuGI::BoneInfo>& boneInfoVec)
{
	boneInfoVec.emplace_back(boneInfo);

	for (int i = 0; i < boneInfo.child.size(); ++i)
	{
		FillFBXBoneInfoVec(boneInfo.child[i], boneInfoVec);
	}
}

void ResourceManager::CreateResourceFromFBX(FBXMeshData& meshData, std::vector<yunuGI::FBXData>& dataVec, yunuGI::FBXData& fbxData)
{
	// Fill FBXData 
	yunuGI::FBXData _data;
	// 1. Mesh Name Set
	_data.meshName = meshData.meshName;
	// 2. Material Set
	yunuGI::MaterialData materialData;
	materialData.materialName = meshData.material.materialName;
	materialData.albedoMap = meshData.material.albedoMap;
	materialData.normalMap = meshData.material.normalMap;
	materialData.armMap = meshData.material.armMap;
	materialData.emissionMap = meshData.material.emissionMap;
	_data.materialVec.emplace_back(materialData);
	// Data Push
	if (fbxData.meshName.empty())
	{
		// 메쉬가 새로 생성 되었을 경우
		dataVec.emplace_back(_data);
	}
	else
	{
		// 이미 메쉬가 있는 경우 머터리얼만 추가
		fbxData.materialVec.emplace_back(materialData);
	}

	// Create Texture
	CreateTexture(materialData.albedoMap);
	CreateTexture(materialData.normalMap);
	CreateTexture(materialData.armMap);
	CreateTexture(materialData.emissionMap);

	// Create Material
	if (this->GetMaterial(materialData.materialName))
	{
		return;
	}
	else
	{
		yunuGI::IMaterial* material = CrateMaterial(materialData.materialName);
		material->SetTexture(yunuGI::Texture_Type::ALBEDO, GetTexture(materialData.albedoMap).get());
		material->SetTexture(yunuGI::Texture_Type::NORMAL, GetTexture(materialData.normalMap).get());
		material->SetTexture(yunuGI::Texture_Type::ARM, GetTexture(materialData.armMap).get());
		material->SetTexture(yunuGI::Texture_Type::EMISSION, GetTexture(materialData.emissionMap).get());
	}
}

void ResourceManager::LoadCubeMesh()
{
	std::shared_ptr<Mesh> cubeMesh = std::make_shared<Mesh>();

	cubeMesh->SetName(L"Cube");

	float w2 = 0.5f;
	float h2 = 0.5f;
	float d2 = 0.5f;

	std::vector<Vertex> vec(24);

	// 앞면
	vec[0] = Vertex(DirectX::SimpleMath::Vector3(-w2, -h2, -d2), DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f), DirectX::SimpleMath::Vector2(0.0f, 1.0f), DirectX::SimpleMath::Vector3(0.0f, 0.0f, -1.0f));
	vec[1] = Vertex(DirectX::SimpleMath::Vector3(-w2, +h2, -d2), DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f), DirectX::SimpleMath::Vector2(0.0f, 0.0f), DirectX::SimpleMath::Vector3(0.0f, 0.0f, -1.0f));
	vec[2] = Vertex(DirectX::SimpleMath::Vector3(+w2, +h2, -d2), DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f), DirectX::SimpleMath::Vector2(1.0f, 0.0f), DirectX::SimpleMath::Vector3(0.0f, 0.0f, -1.0f));
	vec[3] = Vertex(DirectX::SimpleMath::Vector3(+w2, -h2, -d2), DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f), DirectX::SimpleMath::Vector2(1.0f, 1.0f), DirectX::SimpleMath::Vector3(0.0f, 0.0f, -1.0f));
	// 뒷면
	vec[4] = Vertex(DirectX::SimpleMath::Vector3(-w2, -h2, +d2), DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f), DirectX::SimpleMath::Vector2(1.0f, 1.0f), DirectX::SimpleMath::Vector3(0.0f, 0.0f, 1.0f));
	vec[5] = Vertex(DirectX::SimpleMath::Vector3(+w2, -h2, +d2), DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f), DirectX::SimpleMath::Vector2(0.0f, 1.0f), DirectX::SimpleMath::Vector3(0.0f, 0.0f, 1.0f));
	vec[6] = Vertex(DirectX::SimpleMath::Vector3(+w2, +h2, +d2), DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f), DirectX::SimpleMath::Vector2(0.0f, 0.0f), DirectX::SimpleMath::Vector3(0.0f, 0.0f, 1.0f));
	vec[7] = Vertex(DirectX::SimpleMath::Vector3(-w2, +h2, +d2), DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f), DirectX::SimpleMath::Vector2(1.0f, 0.0f), DirectX::SimpleMath::Vector3(0.0f, 0.0f, 1.0f));
	// 윗면
	vec[8] = Vertex(DirectX::SimpleMath::Vector3(-w2, +h2, -d2), DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f), DirectX::SimpleMath::Vector2(0.0f, 1.0f), DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f));
	vec[9] = Vertex(DirectX::SimpleMath::Vector3(-w2, +h2, +d2), DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f), DirectX::SimpleMath::Vector2(0.0f, 0.0f), DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f));
	vec[10] = Vertex(DirectX::SimpleMath::Vector3(+w2, +h2, +d2), DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f), DirectX::SimpleMath::Vector2(1.f, 0.f), DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f));
	vec[11] = Vertex(DirectX::SimpleMath::Vector3(+w2, +h2, -d2), DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f), DirectX::SimpleMath::Vector2(1.f, 1.f), DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f));
	// 아랫면
	vec[12] = Vertex(DirectX::SimpleMath::Vector3(-w2, -h2, -d2), DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f), DirectX::SimpleMath::Vector2(1.0f, 1.0f), DirectX::SimpleMath::Vector3(0.0f, -1.0f, 0.0f));
	vec[13] = Vertex(DirectX::SimpleMath::Vector3(+w2, -h2, -d2), DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f), DirectX::SimpleMath::Vector2(0.0f, 1.0f), DirectX::SimpleMath::Vector3(0.0f, -1.0f, 0.0f));
	vec[14] = Vertex(DirectX::SimpleMath::Vector3(+w2, -h2, +d2), DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f), DirectX::SimpleMath::Vector2(0.0f, 0.0f), DirectX::SimpleMath::Vector3(0.0f, -1.0f, 0.0f));
	vec[15] = Vertex(DirectX::SimpleMath::Vector3(-w2, -h2, +d2), DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f), DirectX::SimpleMath::Vector2(1.0f, 0.0f), DirectX::SimpleMath::Vector3(0.0f, -1.0f, 0.0f));
	// 왼쪽면
	vec[16] = Vertex(DirectX::SimpleMath::Vector3(-w2, -h2, +d2), DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f), DirectX::SimpleMath::Vector2(0.0f, 1.0f), DirectX::SimpleMath::Vector3(-1.0f, 0.0f, 0.0f));
	vec[17] = Vertex(DirectX::SimpleMath::Vector3(-w2, +h2, +d2), DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f), DirectX::SimpleMath::Vector2(0.0f, 0.0f), DirectX::SimpleMath::Vector3(-1.0f, 0.0f, 0.0f));
	vec[18] = Vertex(DirectX::SimpleMath::Vector3(-w2, +h2, -d2), DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f), DirectX::SimpleMath::Vector2(1.0f, 0.0f), DirectX::SimpleMath::Vector3(-1.0f, 0.0f, 0.0f));
	vec[19] = Vertex(DirectX::SimpleMath::Vector3(-w2, -h2, -d2), DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f), DirectX::SimpleMath::Vector2(1.0f, 1.0f), DirectX::SimpleMath::Vector3(-1.0f, 0.0f, 0.0f));
	// 오른쪽면
	vec[20] = Vertex(DirectX::SimpleMath::Vector3(+w2, -h2, -d2), DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f), DirectX::SimpleMath::Vector2(0.0f, 1.0f), DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f));
	vec[21] = Vertex(DirectX::SimpleMath::Vector3(+w2, +h2, -d2), DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f), DirectX::SimpleMath::Vector2(0.0f, 0.0f), DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f));
	vec[22] = Vertex(DirectX::SimpleMath::Vector3(+w2, +h2, +d2), DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f), DirectX::SimpleMath::Vector2(1.0f, 0.0f), DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f));
	vec[23] = Vertex(DirectX::SimpleMath::Vector3(+w2, -h2, +d2), DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f), DirectX::SimpleMath::Vector2(1.0f, 1.0f), DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f));

	std::vector<unsigned int> idx(36);

	// 앞면
	idx[0] = 0; idx[1] = 1; idx[2] = 2;
	idx[3] = 0; idx[4] = 2; idx[5] = 3;
	// 뒷면
	idx[6] = 4; idx[7] = 5; idx[8] = 6;
	idx[9] = 4; idx[10] = 6; idx[11] = 7;
	// 윗면
	idx[12] = 8; idx[13] = 9; idx[14] = 10;
	idx[15] = 8; idx[16] = 10; idx[17] = 11;
	// 아랫면
	idx[18] = 12; idx[19] = 13; idx[20] = 14;
	idx[21] = 12; idx[22] = 14; idx[23] = 15;
	// 왼쪽면
	idx[24] = 16; idx[25] = 17; idx[26] = 18;
	idx[27] = 16; idx[28] = 18; idx[29] = 19;
	// 오른쪽면
	idx[30] = 20; idx[31] = 21; idx[32] = 22;
	idx[33] = 20; idx[34] = 22; idx[35] = 23;

	cubeMesh->SetData(vec, idx);
	CreateMesh(cubeMesh);
}

void ResourceManager::LoadSphereMesh()
{
	std::shared_ptr<Mesh> sphereMesh = std::make_shared<Mesh>();

	sphereMesh->SetName(L"Sphere");

	float radius = 0.5f; // 구의 반지름
	unsigned int stackCount = 20; // 가로 분할
	unsigned int sliceCount = 20; // 세로 분할

	std::vector<Vertex> vec;

	Vertex v;

	// 북극
	v.pos = DirectX::SimpleMath::Vector3{ 0.0f, radius, 0.0f };
	v.color = DirectX::SimpleMath::Vector4{ 1.0f, 1.0f, 1.0f,1.f };
	v.uv = DirectX::SimpleMath::Vector2(0.5f, 0.0f);
	v.normal = v.pos;
	v.normal.Normalize();
	v.tangent = DirectX::SimpleMath::Vector3{ 1.f,0.f,1.f };
	vec.push_back(v);

	float stackAngle = DirectX::XM_PI / stackCount;
	float sliceAngle = DirectX::XM_2PI / sliceCount;

	float deltaU = 1.f / static_cast<float>(sliceCount);
	float deltaV = 1.f / static_cast<float>(stackCount);

	// 고리마다 돌면서 정점을 계산한다 (북극/남극 단일점은 고리가 X)
	for (unsigned int y = 1; y <= stackCount - 1; ++y)
	{
		float phi = y * stackAngle;

		// 고리에 위치한 정점
		for (unsigned int x = 0; x <= sliceCount; ++x)
		{
			float theta = x * sliceAngle;

			v.pos.x = radius * sinf(phi) * cosf(theta);
			v.pos.y = radius * cosf(phi);
			v.pos.z = radius * sinf(phi) * sinf(theta);

			v.uv = DirectX::SimpleMath::Vector2(deltaU * x, deltaV * y);

			v.normal = v.pos;
			v.normal.Normalize();

			v.tangent.x = -radius * sinf(phi) * sinf(theta);
			v.tangent.y = 0.0f;
			v.tangent.z = radius * sinf(phi) * cosf(theta);
			v.tangent.Normalize();

			v.color = DirectX::SimpleMath::Vector4{ 1.0f, 1.0f, 1.0f,1.f };

			vec.push_back(v);
		}
	}

	// 남극
	v.pos = DirectX::SimpleMath::Vector3{ 0.0f, -radius, 0.0f };
	v.color = DirectX::SimpleMath::Vector4{ 1.0f, 1.0f, 1.0f,1.f };
	v.uv = DirectX::SimpleMath::Vector2(0.5f, 1.0f);
	v.normal = v.pos;
	v.normal.Normalize();
	v.tangent = DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f);
	vec.push_back(v);

	std::vector<unsigned int> idx(36);

	// 북극 인덱스
	for (unsigned int i = 0; i <= sliceCount; ++i)
	{
		//  [0]
		//   |  \
		//  [i+1]-[i+2]
		idx.push_back(0);
		idx.push_back(i + 2);
		idx.push_back(i + 1);
	}

	// 몸통 인덱스
	unsigned int ringVertexCount = sliceCount + 1;
	for (unsigned int y = 0; y < stackCount - 2; ++y)
	{
		for (unsigned int x = 0; x < sliceCount; ++x)
		{
			//  [y, x]-[y, x+1]
			//  |		/
			//  [y+1, x]
			idx.push_back(1 + (y)*ringVertexCount + (x));
			idx.push_back(1 + (y)*ringVertexCount + (x + 1));
			idx.push_back(1 + (y + 1) * ringVertexCount + (x));
			//		 [y, x+1]
			//		 /	  |
			//  [y+1, x]-[y+1, x+1]
			idx.push_back(1 + (y + 1) * ringVertexCount + (x));
			idx.push_back(1 + (y)*ringVertexCount + (x + 1));
			idx.push_back(1 + (y + 1) * ringVertexCount + (x + 1));
		}
	}

	// 남극 인덱스
	unsigned int bottomIndex = static_cast<unsigned int>(vec.size()) - 1;
	unsigned int lastRingStartIndex = bottomIndex - ringVertexCount;
	for (unsigned int i = 0; i < sliceCount; ++i)
	{
		//  [last+i]-[last+i+1]
		//  |      /
		//  [bottom]
		idx.push_back(bottomIndex);
		idx.push_back(lastRingStartIndex + i);
		idx.push_back(lastRingStartIndex + i + 1);
	}

	sphereMesh->SetData(vec, idx);
	CreateMesh(sphereMesh);
}

void ResourceManager::LoadRactangleMesh()
{
	std::shared_ptr<Mesh> rectangleMesh = std::make_shared<Mesh>();

	rectangleMesh->SetName(L"Rectangle");

	float w2 = 0.5f;
	float h2 = 0.5f;

	std::vector<Vertex> vec(4);


	// POS COLOR UV TANGENT
	// 앞면
	vec[0] = Vertex(DirectX::SimpleMath::Vector3(-w2, -h2, 0), DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.f, 1.f), DirectX::SimpleMath::Vector2(0.0f, 1.0f), DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f));
	vec[1] = Vertex(DirectX::SimpleMath::Vector3(-w2, +h2, 0), DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.f, 1.f), DirectX::SimpleMath::Vector2(0.0f, 0.0f), DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f));
	vec[2] = Vertex(DirectX::SimpleMath::Vector3(+w2, +h2, 0), DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.f, 1.f), DirectX::SimpleMath::Vector2(1.0f, 0.0f), DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f));
	vec[3] = Vertex(DirectX::SimpleMath::Vector3(+w2, -h2, 0), DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.f, 1.f), DirectX::SimpleMath::Vector2(1.0f, 1.0f), DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f));

	std::vector<unsigned int> idx(6);

	idx[0] = 0; idx[1] = 1; idx[2] = 2;
	idx[3] = 0; idx[4] = 2; idx[5] = 3;

	rectangleMesh->SetData(vec, idx);
	CreateMesh(rectangleMesh);
}

void ResourceManager::LoadPointMesh()
{

}

void ResourceManager::LoadLineMesh()
{

}

void ResourceManager::LoadCapsuleMesh()
{
	std::shared_ptr<Mesh> capsuleMesh = std::make_shared<Mesh>();

	capsuleMesh->SetName(L"Capsule");

	float radius = 0.5f; // 캡슐의 반지름
	float height = 1.0f; // 캡슐의 높이
	int stackCount = 5; // 수평 분할
	int sliceCount = 20; // 수직 분할

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	// 상단 반구 정점
	vertices.push_back(Vertex{
		DirectX::SimpleMath::Vector3{0.0f, radius + height * 0.5f, 0.0f},
		DirectX::SimpleMath::Vector4{1.f, 1.f, 1.f, 1.f} });

	for (int i = 1; i <= stackCount; i++)
	{
		// 윗방향 벡터와의 각도
		float upTheta = DirectX::XM_PI * 0.5f * (i / static_cast<float>(stackCount));

		float xzsize = radius * sinf(upTheta);
		float ysize = radius * cosf(upTheta);

		for (int j = 0; j < sliceCount; j++)
		{
			float zTheta = DirectX::XM_PI * 2.0f * (j / static_cast<float>(sliceCount));

			float x = xzsize * sinf(zTheta);
			float y = ysize + height * 0.5f;
			float z = xzsize * cosf(zTheta);

			vertices.push_back(Vertex{
				DirectX::SimpleMath::Vector3{x, y, z},
				DirectX::SimpleMath::Vector4(1.f, 1.f,1.f,1.f) }
			);
		}
	}

	size_t middleIdx = vertices.size();

	// 하단 반구 정점
	for (int i = stackCount; i >= 1; i--)
	{
		// 윗방향 벡터와의 각도
		float upTheta = DirectX::XM_PI * 0.5f * (i / static_cast<float>(stackCount));

		float xzsize = radius * sinf(upTheta);
		float ysize = radius * cosf(upTheta);

		for (int j = 0; j < sliceCount; j++)
		{
			float zTheta = DirectX::XM_PI * 2.0f * (j / static_cast<float>(sliceCount));

			float x = xzsize * sinf(zTheta);
			float y = ysize + height * 0.5f;
			float z = xzsize * cosf(zTheta);

			vertices.push_back(Vertex{
				DirectX::SimpleMath::Vector3(x, -y, z),
				DirectX::SimpleMath::Vector4(1.f, 1.f,1.f,1.f) }
			);
		}
	}

	vertices.push_back(Vertex{
		DirectX::SimpleMath::Vector3{0.0f, -(radius + height * 0.5f), 0.0f},
		DirectX::SimpleMath::Vector4(1.f, 1.f,1.f,1.f) }
	);

	// 상단 반구 인덱스
	for (int i = 0; i < sliceCount; i++) {
		int a = 0;
		int b = 1 + i;
		int c = 1 + ((i + 1) % sliceCount);

		indices.push_back(a);
		indices.push_back(b);
		indices.push_back(c);
	}

	for (int i = 1; i < stackCount; i++) {
		for (int j = 0; j < sliceCount; j++) {
			int a = 1 + (i - 1) * sliceCount + j;
			int b = 1 + (i - 1) * sliceCount + ((j + 1) % sliceCount);
			int c = 1 + i * sliceCount + j;
			int d = 1 + i * sliceCount + ((j + 1) % sliceCount);

			indices.push_back(a);
			indices.push_back(c);
			indices.push_back(d);

			indices.push_back(a);
			indices.push_back(d);
			indices.push_back(b);
		}
	}

	// 실린더 부분 인덱스
	for (int i = 0; i < sliceCount; i++)
	{
		int a = middleIdx - sliceCount + i;
		int b = middleIdx - sliceCount + ((i + 1) % sliceCount);
		int c = middleIdx + i;
		int d = middleIdx + ((i + 1) % sliceCount);

		indices.push_back(a);
		indices.push_back(c);
		indices.push_back(d);

		indices.push_back(a);
		indices.push_back(d);
		indices.push_back(b);
	}

	// 하단 반구 인덱스
	for (int i = 1; i < stackCount; i++) {
		for (int j = 0; j < sliceCount; j++) {
			int a = middleIdx + (i - 1) * sliceCount + j;
			int b = middleIdx + (i - 1) * sliceCount + ((j + 1) % sliceCount);
			int c = middleIdx + i * sliceCount + j;
			int d = middleIdx + i * sliceCount + ((j + 1) % sliceCount);

			indices.push_back(a);
			indices.push_back(c);
			indices.push_back(d);

			indices.push_back(a);
			indices.push_back(d);
			indices.push_back(b);
		}
	}

	for (int i = 0; i < sliceCount; i++) {
		int a = vertices.size() - 1;
		int b = vertices.size() - 1 - sliceCount + i;
		int c = vertices.size() - 1 - sliceCount + ((i + 1) % sliceCount);

		indices.push_back(b);
		indices.push_back(a);
		indices.push_back(c);
	}

	capsuleMesh->SetData(vertices, indices);
	CreateMesh(capsuleMesh);
}

void ResourceManager::LoadCylinderMesh()
{
	std::shared_ptr<Mesh> cylinderMesh = std::make_shared<Mesh>();

	cylinderMesh->SetName(L"Cylinder");

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	float radius = 0.5f; // 실린더의 반지름
	float height = 1.0f; // 실린더의 높이
	int sliceCount = 20; // 수직 분할

	// 정상 정점
	vertices.push_back(Vertex{
		DirectX::SimpleMath::Vector3{0.0f, height * 0.5f, 0.0f},
		DirectX::SimpleMath::Vector4{1.f, 1.f,1.f,1.f},
		DirectX::SimpleMath::Vector2{ 0.0f, 1.0f },
		DirectX::SimpleMath::Vector3{ 0.0f, 1.0f, 0.0f } }
	);

	// 바닥 정점
	vertices.push_back(Vertex{
		DirectX::SimpleMath::Vector3{0.0f, -height * 0.5f, 0.0f},
		DirectX::SimpleMath::Vector4{1.f, 1.f, 1.f,1.f} ,
		DirectX::SimpleMath::Vector2{ 0.0f, 1.0f },
		DirectX::SimpleMath::Vector3{ 0.0f, 1.0f, 0.0f } }
	);

	// 윗면
	for (int i = 0; i < sliceCount; ++i)
	{
		float zTheta = DirectX::XM_PI * 2.0f * (i / static_cast<float>(sliceCount));

		float x = radius * sinf(zTheta);
		float y = height * 0.5f;
		float z = radius * cosf(zTheta);

		vertices.push_back(Vertex{
			DirectX::SimpleMath::Vector3{x, y, z},
			DirectX::SimpleMath::Vector4{1.f, 1.f,1.f,1.f},
			DirectX::SimpleMath::Vector2{ 0.0f, 1.0f },
			DirectX::SimpleMath::Vector3{ 0.0f, 1.0f, 0.0f } }
		);
	}

	// 밑면
	for (int i = 0; i < sliceCount; ++i)
	{
		float zTheta = DirectX::XM_PI * 2.0f * (i / static_cast<float>(sliceCount));

		float x = radius * sinf(zTheta);
		float y = height * 0.5f;
		float z = radius * cosf(zTheta);

		vertices.push_back(Vertex{
			DirectX::SimpleMath::Vector3{x, -y, z},
			DirectX::SimpleMath::Vector4{1.f, 1.f,1.f,1.f },
			DirectX::SimpleMath::Vector2{ 0.0f, 1.0f },
			DirectX::SimpleMath::Vector3{ 0.0f, 1.0f, 0.0f } }
		);
	}

	// 윗면 인덱스
	for (int i = 0; i < sliceCount; ++i)
	{
		int a = 0;
		int b = 2 + i;
		int c = 2 + ((i + 1) % sliceCount);

		indices.push_back(a);
		indices.push_back(b);
		indices.push_back(c);
	}

	// 옆면 인덱스
	for (int i = 0; i < sliceCount; ++i)
	{
		int a = 2 + i;
		int b = 2 + ((i + 1) % sliceCount);
		int c = 2 + sliceCount + i;
		int d = 2 + sliceCount + ((i + 1) % sliceCount);

		indices.push_back(a);
		indices.push_back(c);
		indices.push_back(d);

		indices.push_back(a);
		indices.push_back(d);
		indices.push_back(b);
	}

	// 밑면 인덱스
	for (int i = 0; i < sliceCount; ++i)
	{
		int a = 1;
		int b = 2 + sliceCount + i;
		int c = 2 + sliceCount + ((i + 1) % sliceCount);

		indices.push_back(b);
		indices.push_back(a);
		indices.push_back(c);
	}

	cylinderMesh->SetData(vertices, indices);
	CreateMesh(cylinderMesh);
}
