#pragma once

#include "Utils.h"
#include "Struct.h"
#include "ModelData.h"

#include "DXTK/SpriteBatch.h"
#include "DXTK/CommonStates.h"

#include <DirectXMath.h>
#include "SimpleMath.h"
using namespace DirectX::PackedVector;



#include "IShader.h"
#include "Material.h"

#include "UIImage.h"

#include <memory>
#include <vector>
#include <unordered_set>
#include <set>
#include <map>

class Mesh;
class Material;
class yunuGI::IShader;
class Animation;
class NailAnimator;

class RenderSystem
{
public:
	static LazyObjects<RenderSystem> Instance;
	friend LazyObjects<RenderSystem>;

public:
	void Init();

	void Render();
	void PushLightData();
	void PushCameraData();
	void RenderObject();
	void RenderSkinned();
	void RenderShadow();
	void RenderLight();
	void RenderFinal();
	void RenderBackBuffer();
	void RenderUI();

	void RenderForward();

	void DrawDeferredInfo();

public:
	void PushStaticRenderableObject(IRenderable* renderable);
	void PopStaticRenderableObject(IRenderable* renderable);

	void PushSkinnedRenderableObject(IRenderable* renderable);
	void PopSkinnedRenderableObject(IRenderable* renderable);

	void PushUIObject(std::shared_ptr<IRenderable> renderable);
	void PopUIObject(std::shared_ptr<IRenderable> renderable);

	void ReSortUIObject(int layer, std::shared_ptr<UIImage> ui);

	void ReSortRenderInfo(IRenderable* renderable, int index);

	void RegisterRenderInfo(IRenderable* renderable, std::shared_ptr<RenderInfo> renderInfo);
	void RegisterSkinnedRenderInfo(IRenderable* renderable, std::shared_ptr<SkinnedRenderInfo> renderInfo);

private:
	std::set<std::shared_ptr<RenderInfo>> deferredSet;
	std::set<std::shared_ptr<RenderInfo>> forwardSet;
	std::set<std::shared_ptr<SkinnedRenderInfo>> skinnedSet;
	
	BoneMatrix finalTM;

	std::unique_ptr<DirectX::SpriteBatch> spriteBatch;
	std::unique_ptr<DirectX::CommonStates> commonStates;

	yunuGI::IShader* ps = nullptr;
	yunuGI::IShader* vs = nullptr;

private:
	//std::unordered_set<std::shared_ptr<IRenderable>> staticRenderableSet;

	std::map<IRenderable*, std::vector<std::shared_ptr<RenderInfo>>> staticMeshRenderInfoMap;
	std::map<IRenderable*, std::vector<std::shared_ptr<SkinnedRenderInfo>>> skinnedMeshRenderInfoMap;

	std::set<std::shared_ptr<IRenderable>, CompareSmartPtr> UIImageSet;
};



