#pragma once

#include <memory>
#include <string>

#include <DirectXMath.h>
#include "SimpleMath.h"
using namespace DirectX::PackedVector;

#include "Material.h"

#include "Mesh.h"

class yunuGI::IMaterial;

class IRenderable
{
public:
	IRenderable();
#pragma region Setter
	void SetWorldTM(const DirectX::SimpleMath::Matrix& wtm) { this->wtm = wtm; };
	void SetMesh(Mesh* mesh)
	{
		this->mesh = mesh;
	}
	void SetMaterial(unsigned int index, yunuGI::IMaterial* material)
	{
		// 새로운 Material이라면
		if (index + 1 > this->materialVec.size())
		{
			this->materialVec.emplace_back(reinterpret_cast<Material*>(material));
		}
		else
		{
			this->materialVec[index] = reinterpret_cast<Material*>(material);
		}
	}
#pragma endregion

#pragma region Getter
	DirectX::SimpleMath::Matrix& GetWorldTM() { return this->wtm; }
	Mesh* GetMesh() 
	{ 
		return this->mesh; 
	}
	Material* GetMaterial(unsigned int index) 
	{
		return materialVec[index]; 
	}
#pragma endregion

protected:
	DirectX::SimpleMath::Matrix wtm;
	Mesh* mesh;
	std::vector<Material*> materialVec;
};

