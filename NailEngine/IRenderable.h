#pragma once

#include <memory>
#include <string>

#include <DirectXMath.h>
#include "SimpleMath.h"
using namespace DirectX::PackedVector;

#include "IMaterial.h"

class IRenderable
{
public:
	IRenderable();
#pragma region Setter
	void SetWorldTM(const DirectX::SimpleMath::Matrix& wtm) { this->wtm = wtm; };
	void SetMeshName(const std::wstring& meshName) { this->meshName = meshName; }
	void SetMaterial(unsigned int index, yunuGI::IMaterial* material)
	{
		// 새로운 Material이라면
		if (index + 1 > this->materialVec.size())
		{
			this->materialVec.emplace_back(material);
		}
		else
		{
			this->materialVec[index] = material;
		}
	}
#pragma endregion

#pragma region Getter
	DirectX::SimpleMath::Matrix& GetWorldTM() { return this->wtm; }
	std::wstring& GetMeshName() { return meshName; }
	const yunuGI::IMaterial* GetMaterial(unsigned int index) 
	{
		return materialVec[index]; 
	}
#pragma endregion

protected:
	DirectX::SimpleMath::Matrix wtm;
	std::wstring meshName;
	std::vector<yunuGI::IMaterial*> materialVec;
};

