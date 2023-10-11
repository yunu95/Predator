#pragma once

#include "Utils.h"

#include <memory>

class Device;
class SwapChain;

class ResourceBuilder
{
public:
	static LazyObjects<ResourceBuilder> Instance;
	friend LazyObjects<ResourceBuilder>;

	std::shared_ptr<Device> device;
	std::shared_ptr<SwapChain> swapChain;
};

