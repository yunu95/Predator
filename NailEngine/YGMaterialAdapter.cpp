#include "YGMaterialAdapter.h"

#include <DirectXMath.h>
#include "SimpleMath.h"
using namespace DirectX::PackedVector;

#include "Struct.h"

#include "NailEngine.h"
#include "ConstantBuffer.h"
#include "ResourceManager.h"
#include "Shader.h"
#include "Texture.h"



unsigned int yunuGIAdapter::MaterialAdapter::id = 0;

yunuGIAdapter::MaterialAdapter::MaterialAdapter()
	: vs{L"DefaultVS.cso"}
	, ps{ L"DefaultPS.cso" }
	, color {1.f,1.f,1.f,1.f} 
{
	id++;
}

yunuGIAdapter::MaterialAdapter::MaterialAdapter(const MaterialAdapter& rhs)
{
	this->materialDesc = rhs.materialDesc;

	this->color = rhs.color;
	this->vs = rhs.vs;
	this->ps = rhs.ps;
	this->textures = rhs.textures;
	this->useTextures = rhs.useTextures;

	id++;
}

void yunuGIAdapter::MaterialAdapter::SetMaterialName(const std::wstring& materialName)
{
	this->materialDesc.materialName = materialName;
}

void yunuGIAdapter::MaterialAdapter::SetVertexShader(const std::wstring& vertexShader)
{
	this->vs = vertexShader;
}

void yunuGIAdapter::MaterialAdapter::SetPixelShader(const std::wstring& pixelShader)
{
	this->ps = pixelShader;
}

void yunuGIAdapter::MaterialAdapter::SetTexture(yunuGI::Texture_Type textureType, const std::wstring& texture)
{
	this->textures[static_cast<int>(textureType)] = texture;
	this->useTextures[static_cast<int>(textureType)] = 1;
}

void yunuGIAdapter::MaterialAdapter::SetColor(const yunuGI::Color& color)
{
	this->color = color;
}

void yunuGIAdapter::MaterialAdapter::SetInt(int index, int val)
{
	this->temp_int[index] = val;
}

const std::wstring& yunuGIAdapter::MaterialAdapter::GetMaterialName() const
{
	return this->materialDesc.materialName;
}

void yunuGIAdapter::MaterialAdapter::SetMaterialDesc(const yunuGI::MaterialDesc& materialDesc)
{
	this->materialDesc = materialDesc;
	this->materialDesc.materialName = materialDesc.materialName;
}

void yunuGIAdapter::MaterialAdapter::PushGraphicsData()
{
	for (unsigned int i = 0; i < textures.size(); ++i)
	{
		if (this->textures[i].empty())
		{
			this->useTextures[i] = 0;
			continue;
		}

		this->useTextures[i] = 1;
		ResourceManager::Instance.Get().GetTexture(this->textures[i])->Bind(i);
	}

	MaterialBuffer materialBuffer;
	materialBuffer.color = reinterpret_cast<DirectX::SimpleMath::Vector4&>(this->color);
	materialBuffer.useTexture = this->useTextures;
	materialBuffer.temp_int = this->temp_int;

	NailEngine::Instance.Get().GetConstantBuffer(1)->PushGraphicsData(&materialBuffer, sizeof(MaterialBuffer),1);

	ResourceManager::Instance.Get().GetShader(vs)->Bind();
	ResourceManager::Instance.Get().GetShader(ps)->Bind();
}

void yunuGIAdapter::MaterialAdapter::UnBindGraphicsData()
{
	for (unsigned int i = 0; i < textures.size(); ++i)
	{
		if (this->textures[i].empty())
		{
			continue;
		}

		ResourceManager::Instance.Get().GetTexture(this->textures[i])->UnBind(i);
	}
}
