#pragma once

#include "ModelData.h"

#include "Animation.h"

#include "Struct.h"

#include <wrl.h>
#include <vector>
#include <array>
#include <string>

#include <d3d11.h>

#include <DirectXMath.h>
#include "SimpleMath.h"
using namespace DirectX::PackedVector;

struct AnimTransform
{
	// 250
	using TransformArrayType = std::array<DirectX::SimpleMath::Matrix, MAX_BONE_COUNT>;
	// 500
	std::array<TransformArrayType, MAX_FRAME_COUNT> transforms;
};

class AnimationGroup
{
public:
	std::vector<std::shared_ptr<Animation>>& GetAnimationVec() { return animationVec; }
	void CreateTexture();
	void SetFBXName(const std::wstring& fbxName) { this->fbxName = fbxName; }
	void Bind();

private:
	void CreateAnimationTransform(unsigned int idx);
	void ReadBone(FBXNode* fbxNode, DirectX::SimpleMath::Matrix parentMatrix, AnimationClip& clip, int animIndex, int frame);

private:
	std::vector<std::shared_ptr<Animation>> animationVec;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;

	std::vector<AnimTransform> animTransformVec;

	std::wstring fbxName;
};

