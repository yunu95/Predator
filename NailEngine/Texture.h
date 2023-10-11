#pragma once

#include <string>
#include <wrl.h>

#include <d3d11.h>

class Texture
{
public:
	void LoadTexture(const std::wstring& texturePath);
	void Bind();

private:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> SRV;
};

