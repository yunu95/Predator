#pragma once

#include "Utils.h"

#include <memory>
#include <unordered_map>

class Shader;
class Mesh;
class Material;

class ResourceManager
{
public:
	static LazyObjects<ResourceManager> Instance;
	friend LazyObjects<ResourceManager>;

public:
	void CreateDefaultResource();

#pragma region Create
	void CreateShader(const std::wstring& shaderPath);
	void CreateMesh(const std::wstring& mesh);
	void CrateMaterial(const std::wstring& materialName);
#pragma endregion

#pragma region Getter
	std::shared_ptr<Material>& GetMaterial(const std::wstring& materialName);
	std::shared_ptr<Shader>& GetShader(const std::wstring& shaderPath);
	std::shared_ptr<Mesh>& GetMesh(const std::wstring& meshName);
#pragma endregion

private:
	void CreateDefaultShader();
	void CreateDefaultMesh();
	void CreateDefaultMaterial();

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
	std::unordered_map<std::wstring, std::shared_ptr<Shader>> shaderMap;
	std::unordered_map<std::wstring, std::shared_ptr<Mesh>> meshMap;
	std::unordered_map<std::wstring, std::shared_ptr<Material>> materialMap;
};

