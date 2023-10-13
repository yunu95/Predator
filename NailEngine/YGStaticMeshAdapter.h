#pragma once
#include "YunuGraphicsInterface.h"
#include "IYunuGIRenderable.h"
#include "YGRenderableAdapter.h"

#include "RenderableManager.h"
#include "StaticMesh.h"
#include "MaterialWrapper.h"

#include <memory>

namespace yunuGIAdapter
{
	class StaticMeshAdapter : public yunuGIAdapter::RenderableAdapter, public yunuGI::IStaticMeshRenderer
	{
	public:
		StaticMeshAdapter() :RenderableAdapter() 
		{
			renderable = std::make_shared<StaticMesh>();
			RenderableManager::Instance.Get().PushRenderableObject(renderable);

			material = std::make_unique<MaterialWrapper>();
			material->SetRenderable(this->renderable);
		}

		~StaticMeshAdapter()
		{
			RenderableManager::Instance.Get().PopRenderableObject(renderable);
		}

		virtual void SetWorldTM(const yunuGI::Matrix4x4& worldTM)
		{
			renderable->SetWorldTM(reinterpret_cast<const DirectX::SimpleMath::Matrix&>(worldTM));
		};

		virtual void SetActive(bool isActive) {};

		virtual void LoadMesh(const char* fileName)
		{
			renderable->SetMeshName(std::wstring{fileName, fileName + strlen(fileName)});
		};

		virtual void SetPickingMode(bool isPickingModeOn) {}

		virtual void SetMaterial(yunuGI::IMaterial* material) override
		{
			if (this->material->IsOrigin())
			{
				this->material->original = reinterpret_cast<yunuGIAdapter::MaterialAdapter*>(material);
			}
			else
			{
				this->material->variation = reinterpret_cast<yunuGIAdapter::MaterialAdapter*>(material);
			}

			renderable->SetMaterial(material);
		};

		virtual yunuGI::IMaterial* GetMaterial()override
		{
			return this->material.get();
		};

	private:
		std::shared_ptr<StaticMesh> renderable;
		std::unique_ptr<MaterialWrapper> material;

	};
}
