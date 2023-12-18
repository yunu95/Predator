#pragma once
#include "YunuGraphicsInterface.h"
#include "IYunuGIRenderable.h"
#include "YGRenderableAdapter.h"

#include "NailAnimator.h"
#include "NailAnimatorManager.h"

#include <memory>

namespace yunuGIAdapter
{
	class AnimatorAdapter : public yunuGIAdapter::RenderableAdapter, public yunuGI::IAnimator
	{
	public:
		AnimatorAdapter() :RenderableAdapter()
		{
			std::shared_ptr<NailAnimator> _animator = std::make_shared<NailAnimator>();
			NailAnimatorManager::Instance.Get().PushAnimator(_animator);

			this->animator = _animator;
		}

		~AnimatorAdapter()
		{
			
		}
		virtual void SetMesh(yunuGI::IMesh* mesh) override
		{

		}

		virtual void PushAnimation(yunuGI::IAnimation* animation) override
		{
			animator->PushAnimation(animation);
		};
		virtual void Play(yunuGI::IAnimation* animation) override
		{
			animator->Play(animation);
		};
		virtual void SetPlaySpeed(float playSpeed) override
		{
			animator->SetPlaySpeed(playSpeed);
		};

		virtual float GetPlaySpeed() override
		{
			return animator->GetPlaySpeed();
		};

		virtual void SetModel(const std::wstring& modelName) override
		{
			animator->SetModel(modelName);
		};

		virtual void SetCurrentFrame(int frame) override
		{
			animator->SetCurrentFrame(frame);
		};

		virtual void SetFrameRatio(float ratio) override
		{
			animator->SetFrameRatio(ratio);
		};

		virtual yunuGI::IAnimation* GetCurrentAnimation() override
		{
			return animator->GetCurrentAnimation();
		};

		virtual unsigned int GetID() override
		{
			return animator->GetID();
		};

		virtual void SetActive(bool isActive)
		{};

		virtual void SetPickingMode(bool isPickingModeOn) {}

	private:
		std::shared_ptr<NailAnimator> animator;

	};
}
