#include "IRenderable.h"

#include "ResourceManager.h"

IRenderable::IRenderable()
{
	this->material = ResourceManager::Instance.Get().GetMaterial(L"DefaultMaterial").get();
}
