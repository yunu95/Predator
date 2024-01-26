#pragma once

#include "IShader.h"

#include "Utils.h"

#include <d3d11.h>

#include <wrl.h>

class Texture;
class Mesh;
class yunuGI::IShader;
class VertexShader;
class PixelShader;

class SkyBoxPass
{
public:
	static LazyObjects<SkyBoxPass> Instance;
	friend LazyObjects<SkyBoxPass>;

public:
	void Init(Texture* texture, Mesh* mesh, yunuGI::IShader* vs, yunuGI::IShader* ps);
	void Render();

	void BuildIrradianceMap();
	void BuildSpecularMap();
	void BuildLUT();

	void BindIBLTexture();

private:
	Texture* texture;
	Mesh* mesh;
	VertexShader* vs;
	PixelShader* ps;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> irradianceSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> prefilteredSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> lutSRV;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
};

