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
			RenderableManager::Instance.Get().PushStaticRenderableObject(renderable);

			std::shared_ptr<MaterialWrapper> material = std::make_shared<MaterialWrapper>(true);
			material->SetRenderable(this->renderable);
			this->materialVec.emplace_back(material);
		}

		~StaticMeshAdapter()
		{
			RenderableManager::Instance.Get().PopStaticRenderableObject(renderable);
		}

		virtual void SetWorldTM(const yunuGI::Matrix4x4& worldTM)
		{
			renderable->SetWorldTM(reinterpret_cast<const DirectX::SimpleMath::Matrix&>(worldTM));
		};

		virtual void SetActive(bool isActive) {};

		virtual void SetMesh(yunuGI::IMesh* mesh) override
		{
			renderable->SetMesh(reinterpret_cast<Mesh*>(mesh));
		};

		virtual void SetPickingMode(bool isPickingModeOn) {}

		virtual void SetMaterial(unsigned int index, yunuGI::IMaterial* material) override
		{
			std::shared_ptr<Material> materialPtr = std::shared_ptr<Material>(reinterpret_cast<Material*>(material));

			// 새로운 Material이라면
			if (index + 1 > this->materialVec.size())
			{
				std::shared_ptr<MaterialWrapper> tempMaterial = std::make_shared<MaterialWrapper>();
				tempMaterial->SetRenderable(this->renderable);
				this->materialVec.emplace_back(tempMaterial);

				if (this->materialVec.back()->IsOrigin())
				{
					this->materialVec.back()->original = materialPtr;
				}
				else
				{
					this->materialVec.back()->variation = materialPtr;
				}
			}
			else
			{
				if (this->materialVec[index]->IsOrigin())
				{
					this->materialVec[index]->original = materialPtr;
				}
				else
				{
					this->materialVec[index]->variation = materialPtr;
				}
			}

			renderable->SetMaterial(index, material);
		};

		virtual yunuGI::IMaterial* GetMaterial(unsigned int index = 0)override
		{
			return this->materialVec[index].get();
		};

	private:
		std::shared_ptr<StaticMesh> renderable;
		std::vector<std::shared_ptr<MaterialWrapper>> materialVec;

	};
}
