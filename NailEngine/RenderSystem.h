#pragma once

#include "Utils.h"

class RenderSystem
{
public:
	static LazyObjects<RenderSystem> Instance;
	friend LazyObjects<RenderSystem>;

public:
	void RenderObject();
};

