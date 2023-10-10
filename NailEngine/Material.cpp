#include "Material.h"

#include "NailEngine.h"
#include "ConstantBuffer.h"

#include "ResourceManager.h"
#include "Shader.h"

Material::Material()
	: materialDesc{}
{

}

Material::~Material()
{

}

void Material::PushGraphicsData()
{
	//NailEngine::Instance.Get().GetConstantBuffer(1)->PushGraphicsData(&materialBuffer, sizeof(MaterialBuffer));

	assert(!vs.empty());
	assert(!ps.empty());

	ResourceManager::Instance.Get().GetShader(vs)->Bind();
	ResourceManager::Instance.Get().GetShader(ps)->Bind();
}