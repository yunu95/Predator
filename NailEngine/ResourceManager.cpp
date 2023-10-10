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
	size_t dotPos = shaderPath.find_last_of(L".");

	std::wstring shaderType = shaderPath.substr(dotPos - 2, 2);
	std::shared_ptr<Shader> shader = nullptr;

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

yunuGI::IMaterial* ResourceManager::CrateMaterial(yunuGI::MaterialDesc& materialDesc)
{
	std::shared_ptr<yunuGIAdapter::MaterialAdapter> material = std::make_shared<yunuGIAdapter::MaterialAdapter>();

	if (materialDesc.materialName.empty())
	{
		materialDesc.materialName = L"DefaultMaterial";
	}

	material->SetMaterialDesc(materialDesc);

	materialMap.insert({ materialDesc.materialName, material });

	return material.get();
}

yunuGIAdapter::MaterialAdapter* ResourceManager::CreateInstanceMaterial(yunuGIAdapter::MaterialAdapter* material)
{
	yunuGIAdapter::MaterialAdapter* _material = new yunuGIAdapter::MaterialAdapter(*material);
	std::shared_ptr<yunuGIAdapter::MaterialAdapter> instanceMaterial(_material);

	yunuGI::MaterialDesc desc{};
	std::wstring materialName = instanceMaterial->GetMaterialName();
	materialName += L"_instance_";
	materialName += std::to_wstring(material->GetID());
	desc.materialName = materialName;

	instanceMaterial->SetMaterialDesc(desc);

	instanceMaterialMap.insert({ desc.materialName, instanceMaterial });

	return instanceMaterial.get();
}

std::shared_ptr<yunuGIAdapter::MaterialAdapter>& ResourceManager::GetMaterial(const std::wstring& materialName)
{
	auto iter = materialMap.find(materialName);
	if (iter == materialMap.end())
	{
		auto iter2 = instanceMaterialMap.find(materialName);
		assert(iter2 != instanceMaterialMap.end());
		return iter2->second;
	}
	else
	{
		return iter->second;
	}
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
#pragma region VS
	CreateShader(L"DefaultVS.cso");
#pragma endregion

#pragma region PS
	CreateShader(L"DefaultPS.cso");
	CreateShader(L"DebugPS.cso");
#pragma endregion
}

void ResourceManager::CreateDefaultMesh()
{
	CreateMesh(L"Cube");
	CreateMesh(L"Sphere");
	CreateMesh(L"Rectangle");
	CreateMesh(L"Rectangle");
	CreateMesh(L"Point");
	CreateMesh(L"Line");
	CreateMesh(L"Capsule");
	CreateMesh(L"Cylinder");
}

void ResourceManager::CreateDefaultMaterial()
{
	// DefaultMaterial
	{
		yunuGI::MaterialDesc desc;
		CrateMaterial(desc);
	}
}

void ResourceManager::LoadCubeMesh()
{
	std::shared_ptr<Mesh> cubeMesh = std::make_shared<Mesh>();

	cubeMesh->SetMeshName(L"Cube");

	float w2 = 0.5f;
	float h2 = 0.5f;
	float d2 = 0.5f;

	std::vector<Vertex> vec(8);
	vec[0] = (Vertex{DirectX::SimpleMath::Vector3{-w2, h2, -d2}, DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f)});
	vec[1] = (Vertex{DirectX::SimpleMath::Vector3{w2, h2, -d2}, DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f)});
	vec[2] = (Vertex{DirectX::SimpleMath::Vector3{-w2, h2, d2}, DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f)});
	vec[3] = (Vertex{ DirectX::SimpleMath::Vector3{w2, h2, d2}, DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f) });
	vec[4] = (Vertex{ DirectX::SimpleMath::Vector3{w2, -h2, -d2}, DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f) });
	vec[5] = (Vertex{ DirectX::SimpleMath::Vector3{-w2, -h2, -d2}, DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f) });
	vec[6] = (Vertex{ DirectX::SimpleMath::Vector3{-w2, -h2, d2}, DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f) });
	vec[7] = (Vertex{ DirectX::SimpleMath::Vector3{w2, -h2, d2}, DirectX::SimpleMath::Vector4(1.f, 1.f, 1.f, 1.f) });

	std::vector<unsigned int> idx(36);

	idx[0] = (0);
	idx[1] = (2);
	idx[2] = (1);
	
	idx[3] = (2);
	idx[4] = (3);
	idx[5] = (1);
	
	idx[6] = (7);
	idx[7] = (3);
	idx[8] = (2);
	
	idx[9] = (2);
	idx[10] = (6);
	idx[11] = (7);
	
	idx[12] = (3);
	idx[13] = (4);
	idx[14] = (1);
	
	idx[15] = (4);
	idx[16] = (3);
	idx[17] = (7);
	
	idx[18] = (5);
	idx[19] = (6);
	idx[20] = (2);
	
	idx[21] = (5);
	idx[22] = (2);
	idx[23] = (0);
	
	idx[24] = (7);
	idx[25] = (6);
	idx[26] = (4);
	
	idx[27] = (6);
	idx[28] = (5);
	idx[29] = (4);
	
	idx[30] = (0);
	idx[31] = (1);
	idx[32] = (4);
	
	idx[33] = (0);
	idx[34] = (4);
	idx[35] = (5);

	cubeMesh->Init(vec, idx);
	meshMap.insert({ L"Cube",cubeMesh });
}

void ResourceManager::LoadSphereMesh()
{
	std::shared_ptr<Mesh> sphereMesh = std::make_shared<Mesh>();

	sphereMesh->SetMeshName(L"Sphere");

	float radius = 0.5f; // 구의 반지름
	unsigned int stackCount = 20; // 가로 분할
	unsigned int sliceCount = 20; // 세로 분할

	std::vector<Vertex> vec;

	Vertex v;

	// 북극
	v.pos = DirectX::SimpleMath::Vector3{ 0.0f, radius, 0.0f };
	v.color = DirectX::SimpleMath::Vector4{ 1.0f, 1.0f, 1.0f,1.f };
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

			v.color = DirectX::SimpleMath::Vector4{ 1.0f, 1.0f, 1.0f,1.f };

			vec.push_back(v);
		}
	}

	// 남극
	v.pos = DirectX::SimpleMath::Vector3{ 0.0f, -radius, 0.0f };
	v.color = DirectX::SimpleMath::Vector4{ 1.0f, 1.0f, 1.0f,1.f };
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

	sphereMesh->Init(vec, idx);
	meshMap.insert({ L"Sphere",sphereMesh });
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
	std::shared_ptr<Mesh> capsuleMesh = std::make_shared<Mesh>();

	capsuleMesh->SetMeshName(L"Capsule");

	float radius = 0.5f; // 캡슐의 반지름
	float height = 1.0f; // 캡슐의 높이
	int stackCount = 5; // 수평 분할
	int sliceCount = 20; // 수직 분할

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	// 상단 반구 정점
	vertices.push_back(Vertex{
		DirectX::SimpleMath::Vector3{0.0f, radius + height * 0.5f, 0.0f},
		DirectX::SimpleMath::Vector4{1.f, 1.f, 1.f, 1.f}});

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
		DirectX::SimpleMath::Vector4(1.f, 1.f,1.f,1.f)}
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

	capsuleMesh->Init(vertices, indices);
	meshMap.insert({ L"Capsule",capsuleMesh });
}

void ResourceManager::LoadCylinderMesh()
{
	std::shared_ptr<Mesh> cylinderMesh = std::make_shared<Mesh>();

	cylinderMesh->SetMeshName(L"Cylinder");

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	float radius = 0.5f; // 실린더의 반지름
	float height = 1.0f; // 실린더의 높이
	int sliceCount = 20; // 수직 분할

	// 정상 정점
	vertices.push_back(Vertex{
		DirectX::SimpleMath::Vector3{0.0f, height * 0.5f, 0.0f}, 
		DirectX::SimpleMath::Vector4{1.f, 1.f,1.f,1.f}}
	);

	// 바닥 정점
	vertices.push_back(Vertex{
		DirectX::SimpleMath::Vector3{0.0f, -height * 0.5f, 0.0f},
		DirectX::SimpleMath::Vector4{1.f, 1.f, 1.f,1.f}}
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
			DirectX::SimpleMath::Vector4{1.f, 1.f,1.f,1.f} }
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
			DirectX::SimpleMath::Vector4{1.f, 1.f,1.f,1.f }}
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

	cylinderMesh->Init(vertices, indices);
	meshMap.insert({ L"Cylinder",cylinderMesh });
}