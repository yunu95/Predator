#pragma once

#include "Utils.h"

#include <d3d11.h>

#include <wrl.h>



class ShadowPass
{
public:
	static LazyObjects<ShadowPass> Instance;
	friend LazyObjects<ShadowPass>;

public:
	void Init();
	void Bind();

	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> GetDSV() { return this->dsv; }

private:
	Microsoft::WRL::ComPtr<ID3D11Texture2D> tex2D;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> dsv;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;

};

