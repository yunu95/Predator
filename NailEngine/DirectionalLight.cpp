#include "DirectionalLight.h"

DirectionalLight::DirectionalLight()
{
	this->lightInfo.lightType = static_cast<unsigned int>(LightType::Directional);
	this->meshName = L"Rectangle";
	this->materialName = L"Deferred_DirectionalLight";
}

void DirectionalLight::SetLightDiffuseColor(DirectX::SimpleMath::Vector4& color)
{
	this->lightInfo.color.diffuse = color;
}

void DirectionalLight::SetLightPosition(DirectX::SimpleMath::Vector4& position)
{
	this->lightInfo.position = position;
}

void DirectionalLight::SetIntensity(float intensity)
{
	this->lightInfo.intensity = intensity;
}

void DirectionalLight::SetActive(bool isActive)
{
	this->isActive = isActive;
}

bool DirectionalLight::IsActive()
{
	return this->isActive;
}

void DirectionalLight::SetLightDirection(DirectX::SimpleMath::Vector4& direction)
{
	this->lightInfo.direction = direction;
}

DirectX::SimpleMath::Vector4& DirectionalLight::GetDirection()
{
	return this->lightInfo.direction;
}

void DirectionalLight::SetWorldTM(const DirectX::SimpleMath::Matrix& wtm)
{
	this->wtm = wtm;
}

DirectX::SimpleMath::Matrix& DirectionalLight::GetWorldTM()
{
	return this->wtm;
}
