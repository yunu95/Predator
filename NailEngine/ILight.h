#pragma once

#include <DirectXMath.h>
#include "SimpleMath.h"
using namespace DirectX::PackedVector;

#include "ResourceManager.h"

#define MAX_LIGHT 50

enum class LightType
{
	Directional,
	Point,
	Spot
};

struct LightColor
{
	DirectX::SimpleMath::Vector4 diffuse{ 1.f,1.f,1.f,1.f };
	DirectX::SimpleMath::Vector4 ambient{ 0.3f,0.3f,0.3f,1.f };
	DirectX::SimpleMath::Vector4 specular{ 1.f,1.f,1.f,1.f };
};

struct LightInfo
{
	LightColor color;
	DirectX::SimpleMath::Vector4 position;
	DirectX::SimpleMath::Vector4 direction;
	unsigned int lightType;
	float range;
	float angle;

	float farPlane = 1.f;
	float nearPlane = 0.1f;
	float intensity = 1.f;
	DirectX::SimpleMath::Vector2 padding;
};

struct LightParams
{
	unsigned int lightCount;
	DirectX::SimpleMath::Vector3 padding;
	LightInfo lights[MAX_LIGHT];
};

class ILight
{
public:
	LightInfo& GetLightInfo() { return this->lightInfo; }
	virtual void SetLightDiffuseColor(DirectX::SimpleMath::Vector4& color) = 0;
	virtual void SetLightPosition(DirectX::SimpleMath::Vector4& position) = 0;
	virtual void SetIntensity(float intensity) = 0;
	virtual void SetActive(bool isActive) = 0;
	virtual bool IsActive() = 0;

	void SetID(int id)
	{
		this->id = id; 
	}
	int GetID() { return this->id; }

	std::wstring& GetMeshName() { return this->meshName; }
	std::wstring& GetMaterialName() { return this->materialName; }
protected:
	int id;

	LightInfo lightInfo;
	std::wstring meshName;
	std::wstring materialName;

	bool isActive = true;
};
