#pragma once

#include "ITexture.h"
#include "ILight.h"

#include "Resource.h"

#include <string>
#include <array>
#include <wrl.h>

#include <d3d11.h>

class Texture : public yunuGI::ITexture, public Resource
{
public:
	void LoadTexture(const std::wstring& texturePath);
	void Bind(unsigned int slot);
	void UnBind(unsigned int slot);

	void CreateFromResource(Microsoft::WRL::ComPtr<ID3D11Texture2D>& tex2D);
	void CreateTexture(const std::wstring& texturePath, unsigned int width, unsigned int height, DXGI_FORMAT format, D3D11_BIND_FLAG bindFlag, int arraySize = 1, int sliceCount = 1);

	void CreateLightMapArray(std::vector<yunuGI::ITexture*> textureVec);

	virtual float GetWidth() override { return width; }
	virtual float GetHeight() override { return height; }

#pragma region Getter
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& GetRTV() { return this->RTV; }
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetSRV() { return this->SRV; }
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>& GetDSV() { return this->DSV; }
	std::array< Microsoft::WRL::ComPtr<ID3D11DepthStencilView>, MAX_POINT_LIGHT>& GetDSVArray() { return dsvArray; }
	Microsoft::WRL::ComPtr<ID3D11Texture2D>& GetTex2D() { return this->tex2D; }
#pragma endregion

private:
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> RTV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> SRV;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> DSV;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> tex2D;

	std::array< Microsoft::WRL::ComPtr<ID3D11DepthStencilView>, MAX_POINT_LIGHT> dsvArray;

	float width;
	float height;
};

