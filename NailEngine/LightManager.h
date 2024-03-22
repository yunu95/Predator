#pragma once
#include "Utils.h"

#include <memory>
#include <vector>
#include <set>
class ILight;

class LightManager
{
public:
	static LazyObjects<LightManager> Instance;
	friend LazyObjects<LightManager>;

public:
	void PushLightInstance(std::shared_ptr<ILight> light);
	void PopLightInstance(std::shared_ptr<ILight> light);

#pragma region Getter
	std::set<ILight*>& GetLightList() { return this->lightSet; }
	unsigned int GetLightCount() { return this->lightSet.size(); }
#pragma endregion

private:
	unsigned int lightCount;
	std::set<ILight*> lightSet;
};

