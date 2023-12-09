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
		virtual void SetMesh(yunuGI::IMesh* mesh) override
		{

		}

		virtual void PushAnimation(yunuGI::IAnimation* animation) override
		{

		};
		virtual void Play(yunuGI::IAnimation* animation) override
		{

		};
		virtual void SetPlaySpeed(float playSpeed) override
		{

		};

		virtual void SetActive(bool isActive)
		{};

		virtual void SetPickingMode(bool isPickingModeOn) {}

	};
}
