#include "IRenderable.h"

#include "ResourceManager.h"

IRenderable::IRenderable()
{
	this->materialVec.emplace_back(reinterpret_cast<Material*>(ResourceManager::Instance.Get().GetMaterial(L"DefaultMaterial").get()));
}
