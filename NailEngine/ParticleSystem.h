#pragma once

#include "ParticleRenderInfo.h"

#include "Struct.h"

#include <list>

class yunuGI::IMaterial;
class Texture;
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
	void SetTexture(yunuGI::ITexture* texture)
	{
		this->texture = static_cast<Texture*>(texture);
	}
	Texture* GetTexture() { return texture; }

private:
	unsigned int maxParticle = 500;
	float duration = 5.f;

	Texture* texture;
};

