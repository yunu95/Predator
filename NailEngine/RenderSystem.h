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

class Mesh;
class Material;
class Animation;
class NailAnimator;

struct RenderInfo
{
	Mesh* mesh;
	Material* material;
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
};



