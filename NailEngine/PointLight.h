#pragma once
#include "ILight.h"

class PointLight : public ILight
{
public:
	PointLight();

public:
	virtual void SetLightDiffuseColor(DirectX::SimpleMath::Vector4& color) override;
	virtual void SetLightPosition(DirectX::SimpleMath::Vector4& position) override;

public:
	void SetRange(float range);
};

