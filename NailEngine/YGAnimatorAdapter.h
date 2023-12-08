#pragma once
#include "YunuGraphicsInterface.h"
#include "IYunuGIRenderable.h"
#include "YGRenderableAdapter.h"

#include <memory>

namespace yunuGIAdapter
{
	class AnimatorAdapter : public yunuGIAdapter::RenderableAdapter, public yunuGI::IAnimator
	{
	public:
		AnimatorAdapter() :RenderableAdapter()
		{

		}

		~AnimatorAdapter()
		{
			
		}
		virtual void PushAnimation(yunuGI::IAnimation* animation) 
		{

		};
		virtual void Play(yunuGI::IAnimation* animation) 
		{

		};
		virtual void SetPlaySpeed(float playSpeed) 
		{

		};

		virtual void SetActive(bool isActive) {};

		virtual void SetPickingMode(bool isPickingModeOn) {}

	};
}
