#include "ResourceManager.h"

#include <assert.h>
#include <filesystem>

#include "SimpleMath.h"
using namespace DirectX::PackedVector;

#include "VertexShader.h"
#include "PixelShader.h"
#include "Mesh.h"
#include "Material.h"

LazyObjects<ResourceManager> ResourceManager::Instance;

void ResourceManager::CreateDefaultResource()
{
	CreateDefaultShader();
	CreateDefaultMesh();
	CreateDefaultMaterial();
}

void ResourceManager::CreateShader(const std::wstring& shaderPath)
{
	std::filesystem::path path(shaderPath);
	std::string folderName = path.parent_path().filename().string();

	std::shared_ptr<Shader> shader = nullptr;

	if (folderName == "VS")
	{
		shader = std::make_shared<VertexShader>();
	}
	else if (folderName == "PS")
	{
		shader = std::make_shared<PixelShader>();
	}

	if (shader != nullptr)
	{
		shader->CreateShader(shaderPath);
	}

	shaderMap.insert({ shaderPath,shader });
}

void ResourceManager::CreateMesh(const std::wstring& mesh)
{
	if (mesh == L"Cube")
	{
		LoadCubeMesh();
	}
	else if (mesh == L"Sphere")
	{
		LoadSphereMesh();
	}
	else if (mesh == L"Rectangle")
	{
		LoadRactangleMesh();
	}
	else if (mesh == L"Point")
	{
		LoadPointMesh();
	}
	else if (mesh == L"Line")
	{
		LoadLineMesh();
	}
	else if (mesh == L"Capsule")
	{
		LoadCapsuleMesh();
	}
	else if (mesh == L"Cylinder")
	{
		LoadCylinderMesh();
	}
}

void ResourceManager::CrateMaterial(const std::wstring& materialName)
{
	std::shared_ptr<Material> material = std::make_shared<Material>();

	material->SetMaterialName(materialName);

	materialMap.insert({ materialName,material });
}

std::shared_ptr<Material>& ResourceManager::GetMaterial(const std::wstring& materialName)
{
	auto iter = materialMap.find(materialName);
	assert(iter != materialMap.end());

	return iter->second;
}

std::shared_ptr<Shader>& ResourceManager::GetShader(const std::wstring& shaderPath)
{
	auto iter = shaderMap.find(shaderPath);
	assert(iter != shaderMap.end());

	return iter->second;
}

std::shared_ptr<Mesh>& ResourceManager::GetMesh(const std::wstring& meshName)
{
	auto iter = meshMap.find(meshName);
	assert(iter != meshMap.end());

	return iter->second;
}

void ResourceManager::CreateDefaultShader()
{
	CreateShader(L"Shader/VS/VertexShader.hlsl");
	CreateShader(L"Shader/PS/PixelShader.hlsl");
}

void ResourceManager::CreateDefaultMesh()
{
	CreateMesh(L"Cube");
}

void ResourceManager::CreateDefaultMaterial()
{
	CrateMaterial(L"DefaultMaterial");
}

void ResourceManager::LoadCubeMesh()
{
	std::shared_ptr<Mesh> cubeMesh = std::make_shared<Mesh>();

	cubeMesh->SetMeshName(L"Cube");

	float w2 = 0.5f;
	float h2 = 0.5f;
	float d2 = 0.5f;

	std::vector<Vertex> vec(24);

	// ¾Õ¸é
	vec[0] = Vertex(DirectX::SimpleMath::Vector4(-w2, -h2, -d2, 1.f), DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f));
	vec[1] = Vertex(DirectX::SimpleMath::Vector4(-w2, -h2, -d2, 1.f), DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f));
	vec[2] = Vertex(DirectX::SimpleMath::Vector4(-w2, -h2, -d2, 1.f), DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f));
	vec[3] = Vertex(DirectX::SimpleMath::Vector4(-w2, -h2, -d2, 1.f), DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f));
	// µÞ¸é
	vec[4] = Vertex(DirectX::SimpleMath::Vector4(-w2, -h2, -d2, 1.f), DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f));
	vec[5] = Vertex(DirectX::SimpleMath::Vector4(-w2, -h2, -d2, 1.f), DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f));
	vec[6] = Vertex(DirectX::SimpleMath::Vector4(-w2, -h2, -d2, 1.f), DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f));
	vec[7] = Vertex(DirectX::SimpleMath::Vector4(-w2, -h2, -d2, 1.f), DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f));
	// À­¸é
	vec[8] = Vertex(DirectX::SimpleMath::Vector4(-w2, -h2, -d2, 1.f), DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f));
	vec[9] = Vertex(DirectX::SimpleMath::Vector4(-w2, -h2, -d2, 1.f), DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f));
	vec[10] = Vertex(DirectX::SimpleMath::Vector4(-w2, -h2, -d2, 1.f), DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f));
	vec[11] = Vertex(DirectX::SimpleMath::Vector4(-w2, -h2, -d2, 1.f), DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f));
	// ¾Æ·§¸é
	vec[12] = Vertex(DirectX::SimpleMath::Vector4(-w2, -h2, -d2, 1.f), DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f));
	vec[13] = Vertex(DirectX::SimpleMath::Vector4(-w2, -h2, -d2, 1.f), DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f));
	vec[14] = Vertex(DirectX::SimpleMath::Vector4(-w2, -h2, -d2, 1.f), DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f));
	vec[15] = Vertex(DirectX::SimpleMath::Vector4(-w2, -h2, -d2, 1.f), DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f));
	// ¿ÞÂÊ¸é 
	vec[16] = Vertex(DirectX::SimpleMath::Vector4(-w2, -h2, -d2, 1.f), DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f));
	vec[17] = Vertex(DirectX::SimpleMath::Vector4(-w2, -h2, -d2, 1.f), DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f));
	vec[18] = Vertex(DirectX::SimpleMath::Vector4(-w2, -h2, -d2, 1.f), DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f));
	vec[19] = Vertex(DirectX::SimpleMath::Vector4(-w2, -h2, -d2, 1.f), DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f));
	// ¿À¸¥ÂÊ¸é
	vec[20] = Vertex(DirectX::SimpleMath::Vector4(-w2, -h2, -d2,1.f), DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f));
	vec[21] = Vertex(DirectX::SimpleMath::Vector4(-w2, -h2, -d2,1.f), DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f));
	vec[22] = Vertex(DirectX::SimpleMath::Vector4(-w2, -h2, -d2,1.f), DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f));
	vec[23] = Vertex(DirectX::SimpleMath::Vector4(-w2, -h2, -d2,1.f), DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f));

	std::vector<unsigned int> idx(36);

	// ¾Õ¸é
	idx[0] = 0; idx[1] = 1; idx[2] = 2;
	idx[3] = 0; idx[4] = 2; idx[5] = 3;
	// µÞ¸é
	idx[6] = 4; idx[7] = 5; idx[8] = 6;
	idx[9] = 4; idx[10] = 6; idx[11] = 7;
	// À­¸é
	idx[12] = 8; idx[13] = 9; idx[14] = 10;
	idx[15] = 8; idx[16] = 10; idx[17] = 11;
	// ¾Æ·§¸é
	idx[18] = 12; idx[19] = 13; idx[20] = 14;
	idx[21] = 12; idx[22] = 14; idx[23] = 15;
	// ¿ÞÂÊ¸é
	idx[24] = 16; idx[25] = 17; idx[26] = 18;
	idx[27] = 16; idx[28] = 18; idx[29] = 19;
	// ¿À¸¥ÂÊ¸é
	idx[30] = 20; idx[31] = 21; idx[32] = 22;
	idx[33] = 20; idx[34] = 22; idx[35] = 23;

	cubeMesh->Init(vec, idx);
	meshMap.insert({ L"Cube",cubeMesh });
}

void ResourceManager::LoadSphereMesh()
{

}

void ResourceManager::LoadRactangleMesh()
{

}

void ResourceManager::LoadPointMesh()
{

}

void ResourceManager::LoadLineMesh()
{

}

void ResourceManager::LoadCapsuleMesh()
{

}

void ResourceManager::LoadCylinderMesh()
{

}