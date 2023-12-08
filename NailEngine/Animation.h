#pragma once
#include "IAnimation.h"

#include "Resource.h"

#include <DirectXMath.h>
#include "SimpleMath.h"
using namespace DirectX::PackedVector;

#include <vector>
#include <string>

struct KeyFrameInfo
{
	std::wstring boneName;
	std::vector<DirectX::SimpleMath::Matrix> srtVec;
};

struct AnimationClip
{
	std::wstring name;
	double duration;
	int totalFrame;
	std::vector<KeyFrameInfo> keyFrameInfoVec;
};

class Animation : public yunuGI::IAnimation, public Resource
{
public:
	void SetAnimationClip(const AnimationClip animationClip) { this->animationClip = animationClip; };

private:
	AnimationClip animationClip;
};

