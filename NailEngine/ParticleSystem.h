#pragma once

#include "Struct.h"

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
	ParticleRenderInfo& GetParticleRenderInfo() { return particleRenderInfo; }

	void SetMaxParticle(unsigned int maxParticle);

private:
	ParticleRenderInfo particleRenderInfo;

	unsigned int maxParticle = 500;


};

