#pragma once

#include "Utils.h"

class RenderSystem
{
public:
	static LazyObjects<RenderSystem> Instance;
	friend LazyObjects<RenderSystem>;

public:
	void PushLightData();
	void PushCameraData();
	void Render();
	void RenderObject();
	void RenderLight();
	void DrawFinal();



	void DrawDeferredInfo();
};

