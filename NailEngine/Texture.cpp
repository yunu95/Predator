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
	else // png, jpg, jpeg, bmp
	{
		DirectX::LoadFromWICFile(texturePath.c_str(), DirectX::WIC_FLAGS_NONE, nullptr, image);
	}

	CreateShaderResourceView(ResourceBuilder::Instance.Get().device->GetDevice().Get(), image.GetImages(),
		image.GetImageCount(), image.GetMetadata(), SRV.GetAddressOf());
}

void Texture::Bind(unsigned int slot)
{
	ResourceBuilder::Instance.Get().device->GetDeviceContext().Get()->PSSetShaderResources(slot, 1, this->SRV.GetAddressOf());
}
