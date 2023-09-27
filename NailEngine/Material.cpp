#include "Material.h"

#include "NailEngine.h"
#include "ConstantBuffer.h"

#include "ResourceManager.h"
#include "Shader.h"

Material::Material()
	:	vs{ L"Shader/VS/VertexShader.hlsl" },
		ps{ L"Shader/PS/PixelShader.hlsl" },
		textures{}
{

}

Material::~Material()
{

}

void Material::PushGraphicsData()
{
	NailEngine::Instance.Get().GetConstantBuffer(1)->PushGraphicsData(&materialBuffer, sizeof(MaterialBuffer));

	ResourceManager::Instance.Get().GetShader(vs)->Bind();
	ResourceManager::Instance.Get().GetShader(ps)->Bind();
}