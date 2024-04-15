#pragma once
#include "YunuGraphicsInterface.h"
#include "IYunuGIRenderable.h"
#include "YGRenderableAdapter.h"

#include "RenderSystem.h"
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
			RenderSystem::Instance.Get().PushStaticRenderableObject(renderable.get());

			std::shared_ptr<MaterialWrapper> material = std::make_shared<MaterialWrapper>(true, 0);
			material->SetRenderable(this->renderable);
			this->materialVec.emplace_back(material);
		}

		~StaticMeshAdapter()
		{
			RenderSystem::Instance.Get().PopStaticRenderableObject(renderable.get());
		}

		virtual void SetWorldTM(const yunuGI::Matrix4x4& worldTM)
		{
			renderable->SetWorldTM(reinterpret_cast<const DirectX::SimpleMath::Matrix&>(worldTM));
		};

		virtual void SetActive(bool isActive)
		{
			renderable->SetActive(isActive);
		};

		virtual void SetMesh(yunuGI::IMesh* mesh) override
		{
			renderable->SetMesh(reinterpret_cast<Mesh*>(mesh));
		};

		virtual bool IsActive() override
		{
			return true;
		}

		virtual yunuGI::IMesh* GetMesh() const override
		{
			return renderable->GetMesh();
		}

		virtual void SetPickingMode(bool isPickingModeOn) {}

		virtual void SetMaterial(unsigned int index, yunuGI::IMaterial* material, bool isOrigin = false) override
		{
			// 새로운 Material이라면
			if (index + 1 > this->materialVec.size())
			{
				std::shared_ptr<MaterialWrapper> tempMaterial = std::make_shared<MaterialWrapper>(index);
				tempMaterial->SetRenderable(this->renderable);
				this->materialVec.emplace_back(tempMaterial);

				if (isOrigin)
				{
					this->materialVec.back()->original = reinterpret_cast<Material*>(material);
					this->materialVec.back()->SetIsOrigin(isOrigin);
				}
				else
				{
					if (this->materialVec.back()->IsOrigin())
					{
						this->materialVec.back()->original = reinterpret_cast<Material*>(material);
					}
					else
					{
						this->materialVec.back()->variation = reinterpret_cast<Material*>(material);
					}
				}
			}
			else
			{
				if (isOrigin)
				{
					this->materialVec[index]->original = reinterpret_cast<Material*>(material);
					this->materialVec[index]->SetIsOrigin(isOrigin);
				}
				else
				{
					if (this->materialVec[index]->IsOrigin())
					{
						this->materialVec[index]->original = reinterpret_cast<Material*>(material);
					}
					else
					{
						this->materialVec[index]->variation = reinterpret_cast<Material*>(material);
					}
				}
			}

			renderable->SetMaterial(index, material);
		}

		virtual int GetMaterialCount() override
		{
			return this->materialVec.size();
		};

		virtual void SetLightMapUVOffset(float x, float y) override
		{
			renderable->SetLightMapUVOffset(x, y);
		};
		virtual void SetLightMapUVScaling(float x, float y) override
		{
			renderable->SetLightMapUVScaling(x, y);
		};
		virtual void SetLightMapUVIndex(int index) override
		{
			renderable->SetLightMapUVIndex(index);
		};
		virtual yunuGI::IMaterial* GetMaterial(unsigned int index = 0, bool isInstance = true)override
		{
			//return this->materialVec[index]->GetVariation(isInstance);
			return this->materialVec[index].get();
		};

	private:
		std::shared_ptr<StaticMesh> renderable;
		std::vector<std::shared_ptr<MaterialWrapper>> materialVec;
		
	};
}
