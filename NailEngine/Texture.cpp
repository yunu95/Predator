#include "Texture.h"

#include <filesystem>

#include "DirectXTex.h"

#include "ResourceBuilder.h"
#include "Device.h"

void Texture::LoadTexture(const std::wstring& texturePath)
{
	DirectX::ScratchImage image;

	std::wstring ext = std::filesystem::path(texturePath).extension();

	if (ext == L".dds" || ext == L".DDS")
	{
		DirectX::LoadFromDDSFile(texturePath.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, image);
	}
	else if (ext == L".tga" || ext == L".TGA")
	{
		DirectX::LoadFromTGAFile(texturePath.c_str(), nullptr, image);
	}
	else if (ext == L".hdr" || ext == L".HDR")
	{
		DirectX::LoadFromHDRFile(texturePath.c_str(), nullptr, image);
	}
	else // png, jpg, jpeg, bmp
	{
		DirectX::LoadFromWICFile(texturePath.c_str(), DirectX::WIC_FLAGS_NONE, nullptr, image);
	}

	CreateShaderResourceView(ResourceBuilder::Instance.Get().device->GetDevice().Get(), image.GetImages(),
		image.GetImageCount(), image.GetMetadata(), SRV.GetAddressOf());


	width = static_cast<float>(image.GetMetadata().width);
	height = static_cast<float>(image.GetMetadata().height);
}

void Texture::Bind(unsigned int slot)
{
	ResourceBuilder::Instance.Get().device->GetDeviceContext()->PSSetShaderResources(slot, 1, this->SRV.GetAddressOf());
}

void Texture::UnBind(unsigned int slot)
{
	ID3D11ShaderResourceView* nullSRV = nullptr;
	ResourceBuilder::Instance.Get().device->GetDeviceContext()->PSSetShaderResources(slot, 1, &nullSRV);
}

void Texture::CreateFromResource(Microsoft::WRL::ComPtr<ID3D11Texture2D>& tex2D)
{
	ResourceBuilder::Instance.Get().device->GetDevice()->CreateRenderTargetView(tex2D.Get(), nullptr, this->RTV.GetAddressOf());
}

void Texture::CreateTexture(const std::wstring& texturePath, unsigned int width, unsigned int height, DXGI_FORMAT format, D3D11_BIND_FLAG bindFlag, int arraySize)
{
	D3D11_TEXTURE2D_DESC texDesc{};
	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = arraySize;
	texDesc.Format = format;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = bindFlag;
	texDesc.CPUAccessFlags = 0;
	if (arraySize == 6)
	{
		texDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
	}
	else
	{
		texDesc.MiscFlags = 0;
	}

	HRESULT hr = ResourceBuilder::Instance.Get().device->GetDevice()->CreateTexture2D(&texDesc, nullptr, this->tex2D.GetAddressOf());

	if (bindFlag & D3D11_BIND_DEPTH_STENCIL)
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
		dsvDesc.Flags = 0;
		dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		if (arraySize == 6)
		{
			dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
			dsvDesc.Texture2DArray.MipSlice = 0;
			dsvDesc.Texture2DArray.ArraySize = arraySize;
			dsvDesc.Texture2DArray.FirstArraySlice = 0;
		}
		else
		{
			dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			dsvDesc.Texture2D.MipSlice = 0;
		}
		ResourceBuilder::Instance.Get().device->GetDevice()->CreateDepthStencilView(this->tex2D.Get(), &dsvDesc, this->DSV.GetAddressOf());

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		if (arraySize == 6)
		{
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		}
		else
		{
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		}
		srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
		srvDesc.Texture2D.MostDetailedMip = 0;
		ResourceBuilder::Instance.Get().device->GetDevice()->CreateShaderResourceView(this->tex2D.Get(), &srvDesc, this->SRV.GetAddressOf());
	}
	else
	{
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc{};
		rtvDesc.Format = texDesc.Format;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		ResourceBuilder::Instance.Get().device->GetDevice()->CreateRenderTargetView(this->tex2D.Get(), &rtvDesc, this->RTV.GetAddressOf());

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		srvDesc.Format = texDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;
		ResourceBuilder::Instance.Get().device->GetDevice()->CreateShaderResourceView(this->tex2D.Get(), &srvDesc, this->SRV.GetAddressOf());
	}
}
