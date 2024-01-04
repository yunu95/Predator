#include "RenderableManager.h"

#include "IRenderable.h"
#include "IShader.h"
#include "Material.h"

LazyObjects<RenderableManager> RenderableManager::Instance;

void RenderableManager::PushStaticRenderableObject(std::shared_ptr<IRenderable> renderable)
{
	this->staticRenderableSet.insert(renderable);
}

void RenderableManager::PopStaticRenderableObject(std::shared_ptr<IRenderable> renderable)
{
	this->staticRenderableSet.erase(renderable);
}

void RenderableManager::PushSkinnedRenderableObject(std::shared_ptr<IRenderable> renderable)
{
	this->skinnedRenderableSet.insert(renderable);
}

void RenderableManager::PopSkinnedRenderableObject(std::shared_ptr<IRenderable> renderable)
{
	this->skinnedRenderableSet.erase(renderable);
}

void RenderableManager::PushUIObject(std::shared_ptr<IRenderable> renderable)
{
	this->UIImageSet.insert(renderable);
}

void RenderableManager::PopUIObject(std::shared_ptr<IRenderable> renderable)
{
	this->UIImageSet.erase(renderable);
}

void RenderableManager::ReSortUIObject(int layer, std::shared_ptr<UIImage> ui)
{
	auto iter = this->UIImageSet.find(ui);

	std::static_pointer_cast<UIImage>(*iter)->layer = layer;
	auto newUI = *iter;
	this->UIImageSet.erase(iter);
	this->UIImageSet.insert(newUI);
}
