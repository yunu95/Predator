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
	void SetMaterial(const yunuGI::IMaterial* material) { this->material = material; }
#pragma endregion

#pragma region Getter
	DirectX::SimpleMath::Matrix& GetWorldTM() { return this->wtm; }
	std::wstring& GetMeshName() { return meshName; }
	const yunuGI::IMaterial* GetMaterial() { return material; }
#pragma endregion

protected:
	DirectX::SimpleMath::Matrix wtm;
	std::wstring meshName;
	const yunuGI::IMaterial* material;
};

