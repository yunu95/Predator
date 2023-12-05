#pragma once
#include "Utils.h"

#include <memory>
#include <vector>
class ILight;

class LightManager
{
public:
	static LazyObjects<LightManager> Instance;
	friend LazyObjects<LightManager>;

public:
	void PushLightInstance(std::shared_ptr<ILight> light);

#pragma region Getter
	std::vector<std::shared_ptr<ILight>>& GetLightList() { return this->lightSet; }
	unsigned int& GetLightCount() { return lightCount; }
#pragma endregion

private:
	unsigned int lightCount;
	std::vector<std::shared_ptr<ILight>> lightSet;
};

