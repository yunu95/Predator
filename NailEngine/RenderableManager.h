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
	void PushStaticRenderableObject(std::shared_ptr<IRenderable> renderable);
	void PopStaticRenderableObject(std::shared_ptr<IRenderable> renderable);

	void PushSKinnedRenderableObject(std::shared_ptr<IRenderable> renderable);
	void PopSKinnedRenderableObject(std::shared_ptr<IRenderable> renderable);

#pragma region Getter
	std::unordered_set<std::shared_ptr<IRenderable>>& GetRenderableSet() { return this->staticRenderableSet; }
#pragma endregion

private:
	std::unordered_set<std::shared_ptr<IRenderable>> staticRenderableSet;
	std::unordered_set<std::shared_ptr<IRenderable>> skinnedRenderableSet;
};

