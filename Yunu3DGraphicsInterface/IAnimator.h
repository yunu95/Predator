#pragma once

#include "IYunuGIRenderable.h"
#include "IAnimation.h"
#include <set>

class yunuGI::IAnimation;

namespace yunuGI
{
	class IAnimator : public yunuGI::IRenderable
	{
	public:
		virtual void PushAnimation(yunuGI::IAnimation* animation) = 0;
		virtual void Play(yunuGI::IAnimation* animation) = 0;
		virtual void SetPlaySpeed(float playSpeed) = 0;
		virtual void SetMesh(yunuGI::IMesh* mesh) = 0;

		virtual void SetWorldTM(const Matrix4x4& tm) {};
		virtual void SetActive(bool isActive) {};

	protected:
		float playSpeed;
		int currentFrame;
		int nextFrame;
		float frameRatio;

		yunuGI::IAnimation* currentAnimation;
		std::set<yunuGI::IAnimation*> animationSet;

		yunuGI::IMesh* mesh;

		unsigned __int64 id;
	};

	struct IAnimatorDesc
	{
	};
}