#pragma once
#include "ILight.h"

class DirectionalLight : public ILight
{
public:
	DirectionalLight();

public:
	virtual void SetLightDiffuseColor(DirectX::SimpleMath::Vector4& color) override;
	virtual void SetLightPosition(DirectX::SimpleMath::Vector4& position) override;
	virtual void SetIntensity(float intensity) override;
	virtual void SetActive(bool isActive) override;
	virtual bool IsActive() override;


public:
	void SetLightDirection(DirectX::SimpleMath::Vector4& direction);
	DirectX::SimpleMath::Vector4& GetDirection();

	void SetWorldTM(const DirectX::SimpleMath::Matrix& wtm);
	DirectX::SimpleMath::Matrix& GetWorldTM();

private:
	DirectX::SimpleMath::Matrix wtm;
};

