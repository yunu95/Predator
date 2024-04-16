#pragma once
#include "IAnimation.h"

#include <wrl.h>
#include <d3d11.h>

#include <DirectXMath.h>
#include "SimpleMath.h"
using namespace DirectX::PackedVector;


#include "Resource.h"

struct KeyFrameInfo
{
	DirectX::SimpleMath::Vector3 pos{0,0,0};
	DirectX::SimpleMath::Quaternion rot{0,0,0,1};
	DirectX::SimpleMath::Vector3 scale{1,1,1};
};

struct AnimationClip
{
	std::wstring name;
	double duration;
	int totalFrame;
	bool isLoop = false;
	float speed=1.f;
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

	virtual void SetPlaySpeed(float playSpeed) override
	{
		this->animationClip.speed = playSpeed;
	};

	virtual float GetPlaySpeed() override
	{
		return this->animationClip.speed;
	};

	AnimationClip& GetAnimationClip() { return animationClip; }

	void SetAnimationIndex(int idx) { this->animationIndex = idx; }
	int GetAnimationIndex() {return animationIndex;	}

private:
	AnimationClip animationClip;
	int animationIndex = -1;
};

