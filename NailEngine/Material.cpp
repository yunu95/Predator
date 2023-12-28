#include "Material.h"

#include "NailEngine.h"
#include "ConstantBuffer.h"

#include "ResourceManager.h"

#include "VertexShader.h"
#include "PixelShader.h"
#include "Texture.h"

Material::Material()
{
	vs = std::static_pointer_cast<VertexShader>(ResourceManager::Instance.Get().GetShader(L"DefaultVS.cso"));
	ps = std::static_pointer_cast<PixelShader>(ResourceManager::Instance.Get().GetShader(L"DefaultPS.cso"));
}

Material::Material(const Material& rhs)
{
	this->color = rhs.color;
	this->vs = rhs.vs;
	this->ps = rhs.ps;
	this->textures = rhs.textures;
	this->useTextures = rhs.useTextures;
	this->name = rhs.name;
}

void Material::SetVertexShader(const yunuGI::IShader* shader)
{
	vs = std::static_pointer_cast<VertexShader>(ResourceManager::Instance.Get().GetShader(shader));
}

void Material::SetPixelShader(const yunuGI::IShader* shader)
{
	ps = std::static_pointer_cast<PixelShader>(ResourceManager::Instance.Get().GetShader(shader));
}

void Material::SetTexture(yunuGI::Texture_Type textureType, const yunuGI::ITexture* texture)
{
	this->textures[static_cast<int>(textureType)] = std::static_pointer_cast<Texture>(ResourceManager::Instance.Get().GetTexture(texture->GetName()));
	this->useTextures[static_cast<int>(textureType)] = 1;
}

void Material::SetColor(const yunuGI::Color& color)
{
	this->color = color;
}

void Material::SetInt(int index, int val)
{
	this->temp_int[index] = val;
}

const yunuGI::IShader* Material::GetPixelShader() const
{
	return this->ps.get();
}

Material::~Material()
{
	int a = 1;
}

void Material::PushGraphicsData()
{
	for (unsigned int i = 0; i < textures.size(); ++i)
	{
		if (this->textures[i] == nullptr ||this->textures[i]->GetName().empty())
		{
			this->useTextures[i] = 0;
			continue;
		}

		this->useTextures[i] = 1;
		this->textures[i]->Bind(i);
	}

	MaterialBuffer materialBuffer;
	materialBuffer.color = reinterpret_cast<DirectX::SimpleMath::Vector4&>(this->color);
	materialBuffer.useTexture = this->useTextures;
	materialBuffer.temp_int = this->temp_int;

	NailEngine::Instance.Get().GetConstantBuffer(1)->PushGraphicsData(&materialBuffer, sizeof(MaterialBuffer), 1);

	assert(vs);
	assert(ps);

	vs->Bind();
	ps->Bind();
}

void Material::UnBindGraphicsData()
{
	for (unsigned int i = 0; i < textures.size(); ++i)
	{
		if (this->textures[i] == nullptr)
		{
			continue;
		}

		this->textures[i]->UnBind(i);
	}
}
