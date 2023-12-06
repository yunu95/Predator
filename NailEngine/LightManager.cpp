#include "LightManager.h"

#include "ILight.h"

LazyObjects<LightManager> LightManager::Instance;

void LightManager::PushLightInstance(std::shared_ptr<ILight> light)
{
	light->SetID(this->lightCount);
	this->lightSet.emplace_back(light);
	this->lightCount++;
}
