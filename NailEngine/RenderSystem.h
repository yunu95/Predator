#pragma once

#include "Utils.h"
#include "Struct.h"
#include "ModelData.h"

#include <DirectXMath.h>
#include "SimpleMath.h"
using namespace DirectX::PackedVector;

#include <vector>

class Mesh;
class Material;
class Animation;

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
	int animatorIndex;
};


class RenderSystem
{
public:
	static LazyObjects<RenderSystem> Instance;
	friend LazyObjects<RenderSystem>;

public:
	void ClearRenderInfo();
	void SortObject();
	void PushLightData();
	void PushCameraData();
	void Render();
	void RenderObject();
	void RenderSkinned();
	void RenderLight();
	void DrawFinal();

	void RenderForward();

	void DrawDeferredInfo();

private:
	void BoneUpdate(const SkinnedRenderInfo& skinnedRenderInfo);
	void ReadBone(FBXNode* fbxNode, DirectX::SimpleMath::Matrix parentMatrix, const std::string& fbxName);

private:
	std::vector<RenderInfo> deferredVec;
	std::vector<RenderInfo> forwardVec;
	std::vector<SkinnedRenderInfo> skinnedVec;

	BoneMatrix finalTM;
};



