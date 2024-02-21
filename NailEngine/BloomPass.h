#pragma once

#include "IShader.h"

#include "Utils.h"

#include <d3d11.h>

#include <wrl.h>
#include <memory>

class Texture;
class Mesh;
class yunuGI::IShader;
class VertexShader;
class PixelShader;

class BloomPass
{
public:
	static LazyObjects<BloomPass> Instance;
	friend LazyObjects<BloomPass>;

public:
	void Init();

	void Bloom();

private:
	void DownSampling4x4_0();
	void DownSampling6x6_0();
	void DownSampling6x6_1();
	void UpSampling6x6_0();
	void UpSampling4x4_0();

	void Blur();

private:
	std::shared_ptr<VertexShader> vs;
	std::shared_ptr<PixelShader> copyPS;

	std::shared_ptr<Mesh> quadMesh;

	std::shared_ptr<Texture> emissiveTexture;
};

