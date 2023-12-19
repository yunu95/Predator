#include "SpotLight.h"

SpotLight::SpotLight()
{
	this->lightInfo.lightType = static_cast<unsigned int>(LightType::Spot);
	this->lightInfo.range = 10.f;
	this->lightInfo.angle = 30.f;
}

void SpotLight::SetLightDiffuseColor(DirectX::SimpleMath::Vector4& color)
{
	this->lightInfo.color.diffuse = color;
}

void SpotLight::SetLightPosition(DirectX::SimpleMath::Vector4& position)
{
	this->lightInfo.position = position;
}

void SpotLight::SetLightDirection(DirectX::SimpleMath::Vector4& direction)
{
	this->lightInfo.direction = direction;
}

DirectX::SimpleMath::Vector4& SpotLight::GetDirection()
{
	return this->lightInfo.direction;
}

void SpotLight::SetAngle(float angle)
{
	this->lightInfo.angle = angle;
}

void SpotLight::SetRange(float range)
{
	this->lightInfo.range = range;
}
