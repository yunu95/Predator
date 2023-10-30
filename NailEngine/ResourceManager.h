#pragma once

#include "YunuGraphicsInterface.h"

#include "Utils.h"

#include <memory>
#include <unordered_map>

#include "YGMaterialAdapter.h"

class Shader;
class Mesh;
class Texture;
class yunuGIAdapter::MaterialAdapter;

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
	yunuGI::IMaterial* CrateMaterial(yunuGI::MaterialDesc& materialDesc);
	yunuGIAdapter::MaterialAdapter* CreateInstanceMaterial(yunuGIAdapter::MaterialAdapter* material);
	void CreateTexture(const std::wstring& texturePath);
#pragma endregion

#pragma region Getter
	std::shared_ptr<yunuGIAdapter::MaterialAdapter>& GetMaterial(const std::wstring& materialName);
	std::shared_ptr<Shader>& GetShader(const std::wstring& shaderPath);
	std::shared_ptr<Mesh>& GetMesh(const std::wstring& meshName);
	std::shared_ptr<Texture>& GetTexture(const std::wstring& textureName);
#pragma endregion

private:
	void CreateDefaultShader();
	void CreateDefaultMesh();
	void CreateDefaultMaterial();
	void CreateDefaultTexture();

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
	std::unordered_map<std::wstring, std::shared_ptr<Texture>> textureMap;
	std::unordered_map<std::wstring, std::shared_ptr<Mesh>> meshMap;
	std::unordered_map<std::wstring, std::shared_ptr<yunuGIAdapter::MaterialAdapter>> materialMap;
	std::unordered_map<std::wstring, std::shared_ptr<yunuGIAdapter::MaterialAdapter>> instanceMaterialMap;
};

