#pragma once
#include "YunuGraphicsInterface.h"
#include "IYunuGIRenderable.h"
#include "YGRenderableAdapter.h"

#include "NailAnimator.h"
#include "NailAnimatorManager.h"
#include <memory>

namespace yunuGIAdapter
{
	class AnimatorAdapter : virtual public yunuGIAdapter::RenderableAdapter, virtual public yunuGI::IAnimator
	{
	public:
		AnimatorAdapter() :RenderableAdapter()
		{
			std::shared_ptr<NailAnimator> _animator = std::make_shared<NailAnimator>();
			NailAnimatorManager::Instance.Get().PushAnimator(_animator);

			this->animator = _animator;
		}

		virtual ~AnimatorAdapter()
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

		//virtual void SetCurrentFrame(int frame) override
		//{
		//	animator->SetCurrentFrame(frame);
		//};

		//virtual void SetFrameRatio(float ratio) override
		//{
		//	animator->SetFrameRatio(ratio);
		//};

		virtual yunuGI::IAnimation* GetCurrentAnimation() override
		{
			return animator->GetCurrentAnimation();
		};

		virtual void ChangeAnimation(yunuGI::IAnimation* animation, float transitionDuration, float transitionSpeed)override
		{
			animator->ChangeAnimation(animation, transitionDuration, transitionSpeed);
		};

		virtual yunuGI::TransitionDesc& GetTransitionDesc() override
		{
			return reinterpret_cast<yunuGI::TransitionDesc&>(animator->GetTransitionDesc());
		};

		virtual void SetCurrentAnimation(yunuGI::IAnimation* animation) override
		{
			animator->SetCurrentAnimation(animation);
		};

		virtual void SetNextAnimation(yunuGI::IAnimation* animation) override
		{
			animator->SetNextAnimation(animation);
		};

		virtual yunuGI::IAnimation* GetNextAnimation() override
		{
			return animator->GetNextAnimation();
		};

		virtual void SetTransitionDesc(yunuGI::TransitionDesc& tansitionDesc) override
		{
			animator->SetTransitionDesc(reinterpret_cast<TransitionDesc&>(tansitionDesc));
		};


		virtual unsigned int GetID() override
		{
			return animator->GetID();
		};

		virtual void SetActive(bool isActive)
		{};
		virtual bool IsActive() override
		{
			return true;
		}
		virtual void SetPickingMode(bool isPickingModeOn) {}

		virtual void SetWorldTM(const yunuGI::Matrix4x4& tm)
		{
			yunuGI::IAnimator::SetWorldTM(tm);
		}

	private:
		std::shared_ptr<NailAnimator> animator;

	};
}
