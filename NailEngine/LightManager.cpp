#include "LightManager.h"

#include "ILight.h"

LazyObjects<LightManager> LightManager::Instance;

void LightManager::PushLightInstance(std::shared_ptr<ILight> light)
{
	light->SetID(this->lightCount);
	this->lightSet.insert(light.get());
	this->lightCount++;
}

void LightManager::PopLightInstance(std::shared_ptr<ILight> light)
{
	//lightSet.erase(std::remove(lightSet.begin(), lightSet.end(), light), lightSet.end());
	lightSet.erase(light.get());
}
