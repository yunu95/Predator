#pragma once

#include <string>
#include <wrl.h>

#include <d3d11.h>

class Texture
{
public:
	void LoadTexture(const std::wstring& texturePath);
	void Bind(unsigned int slot);
	void UnBind(unsigned int slot);

	void CreateFromResource(Microsoft::WRL::ComPtr<ID3D11Texture2D>& tex2D);
	void CreateTexture(const std::wstring& texturePath, unsigned int width, unsigned int height, DXGI_FORMAT format, unsigned int bindFlag);

#pragma region Setter
	void SetName(const std::wstring& name) { this->name = name; }
#pragma endregion

#pragma region Getter
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& GetRTV() { return this->RTV; }
#pragma endregion

private:
	std::wstring name;

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> RTV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> SRV;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> tex2D;
};

