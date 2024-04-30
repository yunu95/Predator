#pragma once

#include "IYunuGIRenderable.h"
#include "IAnimation.h"
#include "YunuVector2.h"
#include <set>

class yunuGI::IAnimation;

namespace yunuGI
{
	class IMesh;

	struct KeyframeDesc
	{
		int animIndex;
		unsigned int currFrame;
		unsigned int nextFrame;
		float ratio;
		float sumTime;
		float speed;
		yunuGI::Vector2 padding;
	};

	struct TransitionDesc
	{
		TransitionDesc()
		{
			curr.animIndex = 0;
			next.animIndex = -1;
		}

		void ClearNextAnimation()
		{
			next.animIndex = -1;
			next.currFrame = 0;
			next.nextFrame = 0;
			next.sumTime = 0;
			transitionTotalTime = 0;
			transitionRatio = 0;
		}

		float transitionDuration;
		float transitionSpeed;
		float transitionTotalTime;
		float transitionRatio;
		KeyframeDesc curr;
		KeyframeDesc next;
	};

	class IAnimator : virtual public yunuGI::IRenderable
	{
	public:
		virtual void PushAnimation(yunuGI::IAnimation* animation) = 0;
		virtual void Play(yunuGI::IAnimation* animation) = 0;
		virtual void SetAnimationFrame(yunuGI::IAnimation* animation, unsigned int frame) = 0;
		virtual void SetPlaySpeed(float playSpeed) = 0;
		virtual float GetPlaySpeed() = 0;
		virtual void SetMesh(yunuGI::IMesh* mesh) = 0;
		virtual void SetModel(const std::wstring& modelName) = 0;

		virtual void SetWorldTM(const Matrix4x4& tm) {};
		virtual void SetActive(bool isActive) {};

		//virtual void SetCurrentFrame(int frame) = 0;
		//virtual void SetFrameRatio(float ratio) = 0;

		virtual yunuGI::IAnimation* GetCurrentAnimation() = 0;
		virtual void SetCurrentAnimation(yunuGI::IAnimation* animation) = 0;
		virtual void SetNextAnimation(yunuGI::IAnimation* animation) = 0;
		virtual yunuGI::IAnimation* GetNextAnimation() = 0;
		virtual void ChangeAnimation(yunuGI::IAnimation* animation, float transitionDuration, float transitionSpeed) = 0;

		virtual TransitionDesc& GetTransitionDesc() = 0;
		virtual void SetTransitionDesc(TransitionDesc& tansitionDesc) = 0;

		virtual unsigned int GetID() = 0;
		virtual  ~IAnimator() {};

	protected:
		/*float playSpeed = 1.f;
		int currentFrame;
		int nextFrame;
		float frameRatio;

		yunuGI::IAnimation* currentAnimation;
		yunuGI::IAnimation* nextAnimation;
		std::set<yunuGI::IAnimation*> animationSet;*/

		TransitionDesc transitionDesc;

		std::wstring modelName;

		unsigned __int64 id;
	};

	struct IAnimatorDesc
	{
	};
}