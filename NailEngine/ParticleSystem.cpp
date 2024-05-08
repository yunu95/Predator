#include "ParticleSystem.h"

#include "ResourceBuilder.h"
#include "ResourceManager.h"
#include "Device.h"

ParticleSystem::ParticleSystem()
{
	texture = ResourceManager::Instance.Get().GetTexture(L"Texture/particle.png").get();
}

void ParticleSystem::SetMaxParticle(unsigned int maxParticle)
{
	this->maxParticle = maxParticle;
}

void ParticleSystem::SetDuration(float duration)
{
	this->duration = duration;
}

float ParticleSystem::GetDuration()
{
	return this->duration;
}

