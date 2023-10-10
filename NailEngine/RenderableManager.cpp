#include "RenderableManager.h"

LazyObjects<RenderableManager> RenderableManager::Instance;

void RenderableManager::PushRenderableObject(std::shared_ptr<IRenderable> renderable)
{
	this->renderableSet.insert(renderable);
}
