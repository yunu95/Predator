#include "ParticleSystem.h"

#include "ResourceBuilder.h"
#include "ResourceManager.h"
#include "Device.h"

ParticleSystem::ParticleSystem()
{
	material = ResourceManager::Instance.Get().GetMaterial(L"ParticleMaterial").get();
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

