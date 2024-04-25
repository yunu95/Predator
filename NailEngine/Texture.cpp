#include "Texture.h"

#include <filesystem>

#include "DirectXTex.h"

#include "ResourceBuilder.h"
#include "Device.h"

void Texture::Release()
{
	if (this->tex2D)
	{
		this->tex2D->Release();
	}
	for (auto& each : dsvArray)
	{
		if (each)
		{
			each->Release();
		}
	}
	if (this->DSV)
	{
		this->DSV->Release();
	}
	if (this->SRV)
	{
		this->SRV->Release();
	}
	if (this->RTV)
	{
		this->RTV->Release();
	}
}

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

	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(texDesc));
	texDesc.Width = image.GetMetadata().width;
	texDesc.Height = image.GetMetadata().height;
	texDesc.MipLevels = image.GetMetadata().mipLevels;
	texDesc.ArraySize = image.GetMetadata().arraySize;
	texDesc.Format = image.GetMetadata().format;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = image.GetMetadata().miscFlags;
	ResourceBuilder::Instance.Get().device->GetDevice()->CreateTexture2D(&texDesc, nullptr, this->tex2D.GetAddressOf());


	CreateShaderResourceView(ResourceBuilder::Instance.Get().device->GetDevice().Get(), image.GetImages(),
		image.GetImageCount(), image.GetMetadata(), SRV.GetAddressOf());


	Microsoft::WRL::ComPtr<ID3D11Resource> resource;
	SRV->GetResource(resource.GetAddressOf());

	resource.As(&this->tex2D);


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

void Texture::CreateTexture(const std::wstring& texturePath, unsigned int width, unsigned int height, DXGI_FORMAT format, D3D11_BIND_FLAG bindFlag, int arraySize, int sliceCount)
{
	this->width = width;
	this->height = height;

	D3D11_TEXTURE2D_DESC texDesc{};
	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = arraySize * sliceCount;
	texDesc.Format = format;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = bindFlag;
	texDesc.CPUAccessFlags = 0;

	if (arraySize * sliceCount >= 6)
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
		if (arraySize * sliceCount > 6)
		{
			dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
			dsvDesc.Texture2DArray.MipSlice = 0;
			dsvDesc.Texture2DArray.ArraySize = arraySize;

			for (int i = 0; i < sliceCount; ++i)
			{
				dsvDesc.Texture2DArray.FirstArraySlice = i * arraySize;

				ResourceBuilder::Instance.Get().device->GetDevice()->CreateDepthStencilView(this->tex2D.Get(), &dsvDesc, this->dsvArray[i].GetAddressOf());
			}
		}
		else
		{
			dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			dsvDesc.Texture2D.MipSlice = 0;
			ResourceBuilder::Instance.Get().device->GetDevice()->CreateDepthStencilView(this->tex2D.Get(), &dsvDesc, this->DSV.GetAddressOf());
		}
		
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		if (arraySize * sliceCount > 6)
		{
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBEARRAY;
			srvDesc.TextureCubeArray.NumCubes = sliceCount;
			srvDesc.TextureCubeArray.First2DArrayFace = 0;
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

void Texture::CreateLightMapArray(std::vector<yunuGI::ITexture*> textureVec)
{
	HRESULT hr;

	UINT texWidth = textureVec[0]->GetWidth();
	UINT texHeight = textureVec[0]->GetHeight();

	// 텍스처 배열의 전체 크기
	UINT arraySize = textureVec.size();

	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(texDesc));
	texDesc.Width = texWidth;
	texDesc.Height = texHeight;
	texDesc.ArraySize = arraySize;
	texDesc.MipLevels = 1;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	hr = ResourceBuilder::Instance.Get().device->GetDevice()->CreateTexture2D(&texDesc, nullptr, this->tex2D.GetAddressOf());

		
	for (UINT sliceIndex = 0; sliceIndex < textureVec.size(); ++sliceIndex)
	{
		// Loop through each mip level
		for (UINT mipLevel = 0; mipLevel < 1; ++mipLevel)
		{
			// Copy data from the corresponding source texture and mipmap level to the current slice and mipmap level of the Texture2DArray
			ResourceBuilder::Instance.Get().device->GetDeviceContext()->CopySubresourceRegion(
				this->tex2D.Get(),                      // Destination (Texture2DArray)
				D3D11CalcSubresource(mipLevel, sliceIndex, 1),
				0, 0, 0,                             // DestX, DestY, DestZ
				static_cast<Texture*>(textureVec[sliceIndex])->GetTex2D().Get(),      // Source resource (Texture2D)
				D3D11CalcSubresource(mipLevel, 0, 1), // SrcSubresource (source mipmap level)
				nullptr                              // pSrcBox (use nullptr to copy the entire resource)
			);
		}
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	srvDesc.Texture2DArray.MostDetailedMip = 0;
	srvDesc.Texture2DArray.MipLevels = 1;
	srvDesc.Texture2DArray.FirstArraySlice = 0;
	srvDesc.Texture2DArray.ArraySize = arraySize;

	// SRV 생성
	hr = ResourceBuilder::Instance.Get().device->GetDevice()->CreateShaderResourceView(tex2D.Get(), &srvDesc, this->SRV.GetAddressOf());
	int a = 1;
}
