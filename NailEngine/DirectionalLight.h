#pragma once
#include "ILight.h"

class DirectionalLight : public ILight
{
public:
	DirectionalLight();

public:
	virtual void SetLightDiffuseColor(DirectX::SimpleMath::Vector4& color) override;
	virtual void SetLightPosition(DirectX::SimpleMath::Vector4& position) override;

public:
	void SetLightDirection(DirectX::SimpleMath::Vector4& direction);
};

