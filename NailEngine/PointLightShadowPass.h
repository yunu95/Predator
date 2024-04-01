#pragma once

#include "Utils.h"

#include <d3d11.h>

#include <wrl.h>

class Texture;
class Mesh;
class VertexShader;
class GeometryShader;
class PixelShader;

class PointLightShadowPass
{
public:
	static LazyObjects<PointLightShadowPass> Instance;
	friend LazyObjects<PointLightShadowPass>;

public:
	void Init();
	void Render(int index, bool isSkinned);
	void EndRender();
	void ClearDSVArray();

private:
	Texture* dsTexture;
	VertexShader* vs;
	VertexShader* skinnedVS;
	GeometryShader* gs;
	PixelShader* ps;
};

