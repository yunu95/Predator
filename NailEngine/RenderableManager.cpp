#include "RenderableManager.h"

#include "IRenderable.h"
#include "IShader.h"
#include "Material.h"

LazyObjects<RenderableManager> RenderableManager::Instance;

void RenderableManager::PushRenderableObject(std::shared_ptr<IRenderable> renderable)
{
	this->renderableSet.insert(renderable);
}

void RenderableManager::PopRenderableObject(std::shared_ptr<IRenderable> renderable)
{
	this->renderableSet.erase(renderable);
}
