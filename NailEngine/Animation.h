#pragma once
#include "IAnimation.h"

#include <DirectXMath.h>
#include "SimpleMath.h"
using namespace DirectX::PackedVector;

//#include "ModelData.h"

#include "Resource.h"

struct KeyFrameInfo
{
	DirectX::SimpleMath::Vector3 pos;
	DirectX::SimpleMath::Quaternion rot;
	DirectX::SimpleMath::Vector3 scale;
};

struct AnimationClip
{
	std::wstring name;
	double duration;
	int totalFrame;
	bool isLoop;
	std::vector<std::vector<KeyFrameInfo>> keyFrameInfoVec;
};

class Animation : public yunuGI::IAnimation, public Resource
{
public:
	void SetAnimationClip(const AnimationClip animationClip) { this->animationClip = animationClip; };

	virtual void SetDuration(float duration) override { animationClip.duration = duration; };
	virtual float GetDuration() override { return animationClip.duration; };

	virtual void SetTotalFrame(int totalFrame) override { animationClip.totalFrame = totalFrame; };
	virtual float GetTotalFrame() override { return animationClip.totalFrame; };

	virtual void SetLoop(bool isLoop) override
	{
		animationClip.isLoop = isLoop;
	};

	virtual bool GetLoop() override
	{
		return animationClip.isLoop;
	};

	AnimationClip& GetAnimationClip() { return animationClip; }

private:
	AnimationClip animationClip;
};

