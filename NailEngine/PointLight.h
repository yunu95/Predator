#pragma once
#include "ILight.h"

#include <DirectXMath.h>
#include "SimpleMath.h"
using namespace DirectX::PackedVector;

class PointLight : public ILight
{
public:
	PointLight();

public:
	virtual void SetLightDiffuseColor(DirectX::SimpleMath::Vector4& color) override;
	virtual void SetLightPosition(DirectX::SimpleMath::Vector4& position) override;
	virtual void SetIntensity(float intensity) override;
	virtual void SetActive(bool isActive) override;
	virtual bool IsActive() override;
public:
	void SetRange(float range);
	void SetWorldTM(const DirectX::SimpleMath::Matrix& wtm);
	DirectX::SimpleMath::Matrix& GetWorldTM();
	DirectX::BoundingSphere GetBoundingSphere(DirectX::SimpleMath::Matrix& wtm);

private:
	DirectX::SimpleMath::Matrix wtm;

};

