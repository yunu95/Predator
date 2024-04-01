#pragma once

#include "Utils.h"

#include <d3d11.h>

#include <wrl.h>

class Texture;
class VertexShader;
class PixelShader;

class ShadowPass
{
public:
	static LazyObjects<ShadowPass> Instance;
	friend LazyObjects<ShadowPass>;

public:
	void Init(Texture* dsTexture, VertexShader* vs, PixelShader* ps);
	void Bind();

private:
	Texture* dsTexture;
	VertexShader* vs;
	PixelShader* ps;
};

