#include "IRenderable.h"

#include "ResourceManager.h"

float nail::IRenderable::objectID = 1.f;

nail::IRenderable::IRenderable()
{
	objectID++;
	this->materialVec.emplace_back(reinterpret_cast<Material*>(ResourceManager::Instance.Get().GetMaterial(L"DefaultMaterial").get()));
}
