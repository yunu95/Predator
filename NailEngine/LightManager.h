#pragma once
#include "Utils.h"

#include <memory>
#include <unordered_set>
class ILight;

class LightManager
{
public:
	static LazyObjects<LightManager> Instance;
	friend LazyObjects<LightManager>;

public:
	void PushLightInstance(std::shared_ptr<ILight> light);

#pragma region Getter
	std::unordered_set<std::shared_ptr<ILight>>& GetLightList() { return this->lightSet; }
	unsigned int& GetLightCount() { return lightCount; }
#pragma endregion

private:
	unsigned int lightCount;
	std::unordered_set<std::shared_ptr<ILight>> lightSet;
};

