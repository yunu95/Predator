#pragma once
#include "ILight.h"

class SpotLight : public ILight
{
public:
	SpotLight();

public:
	virtual void SetLightDiffuseColor(DirectX::SimpleMath::Vector4& color) override;
	virtual void SetLightPosition(DirectX::SimpleMath::Vector4& position) override;
	virtual void SetIntensity(float intensity) override
	{
		this->lightInfo.intensity = intensity;
	};

public:
	void SetLightDirection(DirectX::SimpleMath::Vector4& direction);
	DirectX::SimpleMath::Vector4& GetDirection();
	void SetAngle(float angle);
	void SetRange(float range);

};
