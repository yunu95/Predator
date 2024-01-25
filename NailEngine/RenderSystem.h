#pragma once

#include "Utils.h"
#include "Struct.h"
#include "ModelData.h"

#include "DXTK/SpriteBatch.h"
#include "DXTK/CommonStates.h"

#include <DirectXMath.h>
#include "SimpleMath.h"
using namespace DirectX::PackedVector;

#include <vector>
#include <memory>

#include "IShader.h"
#include "Material.h"

#include "UIImage.h"

#include <memory>
#include <unordered_set>
#include <set>

class Mesh;
class Material;
class yunuGI::IShader;
class Animation;
class NailAnimator;

struct RenderInfo
{
	Mesh* mesh;
	Material* material;
	Material shadowMaterial;
	unsigned int materialIndex;
	DirectX::SimpleMath::Matrix wtm;
};

struct SkinnedRenderInfo
{
	RenderInfo renderInfo;
	std::wstring modelName;
	std::shared_ptr<NailAnimator> animator;
	TransitionDesc transitionDesc;
};


class RenderSystem
{
public:
	static LazyObjects<RenderSystem> Instance;
	friend LazyObjects<RenderSystem>;

public:
	void Init();

	void ClearRenderInfo();
	void SortObject();
	void PushLightData();
	void PushCameraData();
	void Render();
	void RenderObject();
	void RenderSkinned();
	void RenderShadow();
	void RenderLight();
	void DrawFinal();
	void RenderUI();

	void RenderForward();

	void DrawDeferredInfo();

public:
	void PushStaticRenderableObject(std::shared_ptr<IRenderable> renderable);
	void PopStaticRenderableObject(std::shared_ptr<IRenderable> renderable);

	void PushSkinnedRenderableObject(std::shared_ptr<IRenderable> renderable);
	void PopSkinnedRenderableObject(std::shared_ptr<IRenderable> renderable);

	void PushUIObject(std::shared_ptr<IRenderable> renderable);
	void PopUIObject(std::shared_ptr<IRenderable> renderable);

	void ReSortUIObject(int layer, std::shared_ptr<UIImage> ui);


private:
	//void BoneUpdate(const SkinnedRenderInfo& skinnedRenderInfo);
	//void ReadBone(FBXNode* fbxNode, DirectX::SimpleMath::Matrix parentMatrix, const std::string& fbxName, std::shared_ptr<NailAnimator> animator);

private:
	std::vector<RenderInfo> deferredVec;
	std::vector<RenderInfo> forwardVec;
	std::vector<SkinnedRenderInfo> skinnedVec;

	BoneMatrix finalTM;

	std::unique_ptr<DirectX::SpriteBatch> spriteBatch;
	std::unique_ptr<DirectX::CommonStates> commonStates;

	yunuGI::IShader* ps = nullptr;
	yunuGI::IShader* vs = nullptr;

private:
	std::unordered_set<std::shared_ptr<IRenderable>> staticRenderableSet;
	std::unordered_set<std::shared_ptr<IRenderable>> skinnedRenderableSet;
	std::set<std::shared_ptr<IRenderable>, CompareSmartPtr> UIImageSet;
};



