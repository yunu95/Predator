#pragma once

#include "IYunuGIRenderable.h"
#include "IAnimation.h"
#include <set>

class yunuGI::IAnimation;

namespace yunuGI
{
	class IMesh;

	class IAnimator : public yunuGI::IRenderable
	{
	public:
		virtual void PushAnimation(yunuGI::IAnimation* animation) = 0;
		virtual void Play(yunuGI::IAnimation* animation) = 0;
		virtual void SetPlaySpeed(float playSpeed) = 0;
		virtual float GetPlaySpeed() = 0;
		virtual void SetMesh(yunuGI::IMesh* mesh) = 0;
		virtual void SetModel(const std::wstring& modelName) = 0;

		virtual void SetWorldTM(const Matrix4x4& tm) {};
		virtual void SetActive(bool isActive) {};

		virtual void SetCurrentFrame(int frame) = 0;
		virtual void SetFrameRatio(float ratio) = 0;

		virtual yunuGI::IAnimation* GetCurrentAnimation() = 0;

		virtual unsigned int GetID() = 0;

	protected:
		float playSpeed = 1.f;
		int currentFrame;
		int nextFrame;
		float frameRatio;

		yunuGI::IAnimation* currentAnimation;
		std::set<yunuGI::IAnimation*> animationSet;

		std::wstring modelName;

		unsigned __int64 id;
	};

	struct IAnimatorDesc
	{
	};
}