#include "ParticleSystem.h"

#include "ResourceBuilder.h"
#include "Device.h"

ParticleSystem::ParticleSystem()
{
	
}

void ParticleSystem::SetMaxParticle(unsigned int maxParticle)
{
	this->maxParticle = maxParticle;
}
