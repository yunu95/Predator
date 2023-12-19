#pragma once
#include "Utils.h"

#include <memory>
#include <unordered_set>

class IRenderable;


class RenderableManager
{
public:
	static LazyObjects<RenderableManager> Instance;
	friend LazyObjects<RenderableManager>;

public:
	void PushRenderableObject(std::shared_ptr<IRenderable> renderable);
	void PopRenderableObject(std::shared_ptr<IRenderable> renderable);

#pragma region Getter
	std::unordered_set<std::shared_ptr<IRenderable>>& GetRenderableSet() { return this->renderableSet; }
#pragma endregion

private:
	std::unordered_set<std::shared_ptr<IRenderable>> renderableSet;
};

