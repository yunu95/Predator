#include "IRenderable.h"

#include "ResourceManager.h"

float IRenderable::objectID = 1.f;

IRenderable::IRenderable()
{
	objectID++;
	this->materialVec.emplace_back(reinterpret_cast<Material*>(ResourceManager::Instance.Get().GetMaterial(L"DefaultMaterial").get()));
}
