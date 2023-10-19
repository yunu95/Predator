#include "PointLight.h"

PointLight::PointLight()
{
	this->lightInfo.lightType = static_cast<unsigned int>(LightType::Point);
	this->lightInfo.range = 1.f;
}

void PointLight::SetLightDiffuseColor(DirectX::SimpleMath::Vector4& color)
{
	this->lightInfo.color.diffuse = color;
	this->lightInfo.color.specular = color;
}

void PointLight::SetLightPosition(DirectX::SimpleMath::Vector4& position)
{
	this->lightInfo.position = position;
}

void PointLight::SetRange(float range)
{
	this->lightInfo.range = range;
}
