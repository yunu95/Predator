#pragma once

#include "Utils.h"

#include <DirectXMath.h>
#include "SimpleMath.h"
using namespace DirectX::PackedVector;

class NailCamera
{
public:
	static LazyObjects<NailCamera> Instance;
	friend LazyObjects<NailCamera>;

public:
	void SetWorldTM(const DirectX::SimpleMath::Matrix wtm);
	void SetFOV(float fov);
	void SetNear(float cameraNear);
	void SetFar(float cameraFar);
	void SetResolution(float width, float height);
	void SetAsMain();

	DirectX::SimpleMath::Matrix& GetVTM() { return this->vtm; }
	DirectX::SimpleMath::Matrix& GetPTM() { return this->ptm; }

private:
	DirectX::SimpleMath::Matrix wtm;
	DirectX::SimpleMath::Matrix vtm;
	DirectX::SimpleMath::Matrix ptm;
	float fov = DirectX::XM_PI / 4.f;
	float cameraNear = 1.f;
	float cameraFar = 1000.f;
	float width = 1280;
	float height = 800;

};

