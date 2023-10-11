#include "YGMaterialAdapter.h"

#include <DirectXMath.h>
#include "SimpleMath.h"
using namespace DirectX::PackedVector;

#include "NailEngine.h"
#include "ConstantBuffer.h"
#include "ResourceManager.h"
#include "Shader.h"

#include "Struct.h"

unsigned int yunuGIAdapter::MaterialAdapter::id = 0;

yunuGIAdapter::MaterialAdapter::MaterialAdapter()
	: vs{L"DefaultVS.cso"}
	, ps{ L"DefaultPS.cso" }
	, color{1.f,1.f,1.f,1.f}
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

	id++;
}

void yunuGIAdapter::MaterialAdapter::SetMaterialName(const std::wstring& materialName)
{
	if (this->materialDesc.materialName != materialName)
	{
		
	}

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

void yunuGIAdapter::MaterialAdapter::SetTexture(const int index, const std::wstring& texture)
{
	this->textures[index] = texture;
}

void yunuGIAdapter::MaterialAdapter::SetColor(const yunuGI::Color& color)
{
	this->color = color;
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
	MaterialBuffer materialBuffer;
	materialBuffer.color = reinterpret_cast<DirectX::SimpleMath::Vector4&>(this->color);
	NailEngine::Instance.Get().GetConstantBuffer(1)->PushGraphicsData(&materialBuffer, sizeof(MaterialBuffer),1);

	ResourceManager::Instance.Get().GetShader(vs)->Bind();
	ResourceManager::Instance.Get().GetShader(ps)->Bind();
}
