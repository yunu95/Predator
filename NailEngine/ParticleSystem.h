#pragma once

#include "ParticleRenderInfo.h"

#include "Struct.h"

#include <list>

class yunuGI::IMaterial;

struct ParticleInfo
{
	DirectX::SimpleMath::Vector3 position;
	DirectX::SimpleMath::Vector3 direction;
	float speed;
};

class ParticleSystem
{
public:
	ParticleSystem();

	void SetMaxParticle(unsigned int maxParticle);
	void SetDuration(float duration);
	float GetDuration();

	yunuGI::IMaterial* GetMaterial() { return material; }

private:
	unsigned int maxParticle = 500;
	float duration = 5.f;


	yunuGI::IMaterial* material;
};

