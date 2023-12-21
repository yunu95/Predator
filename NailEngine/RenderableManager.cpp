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
