#include "LightManager.h"

#include "ILight.h"

LazyObjects<LightManager> LightManager::Instance;

void LightManager::PushLightInstance(std::shared_ptr<ILight> light)
{
	light->SetID(this->lightCount);
	this->lightSet.emplace_back(light);
	this->lightCount++;
}

void LightManager::PopLightInstance(std::shared_ptr<ILight> light)
{
	lightSet.erase(std::remove(lightSet.begin(), lightSet.end(), light), lightSet.end());
}
