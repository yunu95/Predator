#include "IRenderable.h"

#include "ResourceManager.h"

IRenderable::IRenderable()
{
	this->materialVec.emplace_back(ResourceManager::Instance.Get().GetMaterial(L"DefaultMaterial").get());
}
