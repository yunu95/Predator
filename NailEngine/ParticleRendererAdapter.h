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

		virtual void SetMaterial(unsigned int index, yunuGI::IMaterial* material, bool isOrigin = false) override
		{
		
		}

		virtual yunuGI::IMaterial* GetMaterial(unsigned int index = 0, bool isInstance = true) override
		{
			return particleSystem->GetMaterial();
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
		virtual void SetParticleInfoList(std::list<yunuGI::ParticleRenderInfo>& particleRenderInfo) override
		{
			InstancingManager::Instance.Get().RegisterParticleRenderInfo(particleSystem.get(), &particleRenderInfo);
		};
	private:
		std::shared_ptr<ParticleSystem> particleSystem;

	};
}
