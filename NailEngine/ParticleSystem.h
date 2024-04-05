#pragma once

#include "ParticleRenderInfo.h"

#include "Struct.h"

#include <list>

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

private:
	unsigned int maxParticle = 500;
	float duration = 5.f;

};

