#include "DirectionalLight.h"

DirectionalLight::DirectionalLight()
{
	this->lightInfo.lightType = static_cast<unsigned int>(LightType::Directional);
}

void DirectionalLight::SetLightDiffuseColor(DirectX::SimpleMath::Vector4& color)
{
	this->lightInfo.color.diffuse = color;
}

void DirectionalLight::SetLightPosition(DirectX::SimpleMath::Vector4& position)
{
	this->lightInfo.position = position;
}

void DirectionalLight::SetLightDirection(DirectX::SimpleMath::Vector4& direction)
{
	this->lightInfo.direction = direction;
}

DirectX::SimpleMath::Vector4& DirectionalLight::GetDirection()
{
	return this->lightInfo.direction;
}
