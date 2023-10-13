#pragma once

#include <DirectXMath.h>
#include "SimpleMath.h"
using namespace DirectX::PackedVector;

#define MAX_LIGHT 50

enum class LightType
{
	Directional,
	Point,
	Spot
};

struct LightColor
{
	DirectX::SimpleMath::Vector4 diffuse{1.f,0.f,0.f,1.f};
	DirectX::SimpleMath::Vector4 ambient{ 0.f,0.f,0.f,0.f };
	DirectX::SimpleMath::Vector4 specular{ 0.f,0.f,0.f,0.f };
};

struct LightInfo
{
	unsigned int lightType;
	LightColor color;
	DirectX::SimpleMath::Vector4 position;
	DirectX::SimpleMath::Vector4 direction;
	float range;
	float angle;
	float padding;
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

protected:
	LightInfo lightInfo;
};