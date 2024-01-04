#pragma once
#include "Utils.h"

#include "UIImage.h"

#include <memory>
#include <unordered_set>
#include <set>

class IRenderable;

class RenderableManager
{
public:
	static LazyObjects<RenderableManager> Instance;
	friend LazyObjects<RenderableManager>;

public:
	void PushStaticRenderableObject(std::shared_ptr<IRenderable> renderable);
	void PopStaticRenderableObject(std::shared_ptr<IRenderable> renderable);

	void PushSkinnedRenderableObject(std::shared_ptr<IRenderable> renderable);
	void PopSkinnedRenderableObject(std::shared_ptr<IRenderable> renderable);

	void PushUIObject(std::shared_ptr<IRenderable> renderable);
	void PopUIObject(std::shared_ptr<IRenderable> renderable);

	void ReSortUIObject(int layer, std::shared_ptr<UIImage> ui);

#pragma region Getter
	std::unordered_set<std::shared_ptr<IRenderable>>& GetStaticRenderableSet() { return this->staticRenderableSet; }
	std::unordered_set<std::shared_ptr<IRenderable>>& GetSKinnedRenderableSet() { return this->skinnedRenderableSet; }
	std::set<std::shared_ptr<IRenderable>,CompareSmartPtr>& GetUIImageSet() { return this->UIImageSet; }
#pragma endregion

private:
	std::unordered_set<std::shared_ptr<IRenderable>> staticRenderableSet;
	std::unordered_set<std::shared_ptr<IRenderable>> skinnedRenderableSet;
	std::set<std::shared_ptr<IRenderable>, CompareSmartPtr> UIImageSet;
};

