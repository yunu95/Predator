#include "LightManager.h"

LazyObjects<LightManager> LightManager::Instance;

void LightManager::PushLightInstance(std::shared_ptr<ILight> light)
{
	this->lightSet.insert(light);
	this->lightCount++;
}
