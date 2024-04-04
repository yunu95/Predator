#pragma once
#include "YunuGraphicsInterface.h"
#include "IYunuGIRenderable.h"
#include "YGRenderableAdapter.h"

#include "RenderSystem.h"
#include "ParticleSystem.h"
#include "MaterialWrapper.h"

#include <memory>

namespace yunuGIAdapter
{
	class ParticleRendererAdapter : public yunuGIAdapter::RenderableAdapter, public yunuGI::IParticleRenderer
	{
	public:
		ParticleRendererAdapter() : RenderableAdapter()
		{
			particleSystem = std::make_shared<ParticleSystem>();
			RenderSystem::Instance.Get().PushParticleSystem(particleSystem.get());
		}

		~ParticleRendererAdapter()
		{
		}

		virtual void SetWorldTM(const yunuGI::Matrix4x4& worldTM)
		{
		};

		virtual void SetActive(bool isActive)
		{
		};

		virtual void SetMesh(yunuGI::IMesh* mesh) override
		{
		};

		virtual bool IsActive() override
		{
			return true;
		}

		virtual yunuGI::IMesh* GetMesh() const override
		{
			return nullptr;
		}

		virtual void SetPickingMode(bool isPickingModeOn) {}

		virtual void SetMaterial(unsigned int index, yunuGI::IMaterial* material) override
		{
		
		}

		virtual int GetMaterialCount() override
		{
			return 0;
		};

		virtual void SetDuration(float duration) override
		{
			particleSystem->SetDuration(duration);
		};

		virtual float GetDuration() override
		{
			return particleSystem->GetDuration();
		};

	private:
		std::shared_ptr<ParticleSystem> particleSystem;
	};
}
