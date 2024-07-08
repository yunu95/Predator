#pragma once
#include "YunuGraphicsInterface.h"
#include "IYunuGIRenderable.h"
#include "YGRenderableAdapter.h"

#include "RenderSystem.h"
#include "SKinnedMesh.h"
#include "MaterialWrapper.h"

#include <memory>

namespace yunuGIAdapter
{
	class SkinnedMeshAdapter : public yunuGIAdapter::RenderableAdapter, public yunuGI::ISkinnedMesh
	{
	public:
		SkinnedMeshAdapter() :RenderableAdapter()
		{
			renderable = std::make_shared<SkinnedMesh>();

			std::shared_ptr<MaterialWrapper> material = std::make_shared<MaterialWrapper>(false, 0);
			material->SetRenderable(this->renderable);
			//renderable->SetMaterial(0, material->GetMaterial());
			this->materialVec.emplace_back(material);

			/// here ok
			//RenderSystem::Instance.Get().PushSkinnedRenderableObject(renderable.get());
			InstancingManager::Instance.Get().RegisterSkinnedDeferredData(static_cast<SkinnedMesh*>(renderable.get())->renderInfoVec[0]);
		}

		~SkinnedMeshAdapter()
		{

			for (int i = 0; i < static_cast<SkinnedMesh*>(renderable.get())->renderInfoVec.size(); ++i)
			{
				if (static_cast<SkinnedMesh*>(renderable.get())->renderInfoVec[i]->renderInfo.material->GetPixelShader()->GetShaderInfo().shaderType == yunuGI::ShaderType::Deferred)
				{
					InstancingManager::Instance.Get().PopSkinnedDeferredData(static_cast<SkinnedMesh*>(renderable.get())->renderInfoVec[i]);
				}
				else
				{
					InstancingManager::Instance.Get().PopSkinnedForwardData(static_cast<SkinnedMesh*>(renderable.get())->renderInfoVec[i]);
				}
			}
		}

		virtual void SetBone(std::wstring fbxName) override
		{
			renderable->SetBone(fbxName);
		}

		virtual void SetWorldTM(const yunuGI::Matrix4x4& worldTM)
		{
			renderable->SetWorldTM(reinterpret_cast<const DirectX::SimpleMath::Matrix&>(worldTM));
		};

		virtual void SetActive(bool isActive)
		{
			renderable->SetActive(isActive);
		};
		virtual bool IsActive() override
		{
			return true;
		}
		virtual void SetMesh(yunuGI::IMesh* mesh)  override
		{
			renderable->SetMesh(reinterpret_cast<Mesh*>(mesh));
		};

		virtual yunuGI::IMesh* GetMesh() const override
		{
			return renderable->GetMesh();
		};

		virtual void SetAnimatorIndex(int animatorIndex) override
		{
			renderable->SetAnimatorIndex(animatorIndex);
		};

		virtual void SetPickingMode(bool isPickingModeOn) {}

		virtual void SetMaterial(unsigned int index, yunuGI::IMaterial* material, bool isOrigin = false) override
		{
			material = (material)->GetMaterial();

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
						renderable->SetMaterial(index, material);
						this->materialVec.back()->original = reinterpret_cast<Material*>(material);
					}
					else
					{
						renderable->SetMaterial(index, material);
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
						renderable->SetMaterial(index, material);
						this->materialVec[index]->original = reinterpret_cast<Material*>(material);
					}
					else
					{
						renderable->SetMaterial(index, material);
						this->materialVec[index]->variation = reinterpret_cast<Material*>(material);
					}
				}

				
			}

			renderable->SetMaterial(index, material);
		};

		virtual yunuGI::IMaterial* GetMaterial(unsigned int index = 0, bool isInstance = true)override
		{
			if (isInstance)
			{
				return this->materialVec[index].get();
			}
			else
			{
				return this->materialVec[index].get()->GetMaterial();
			}
		};

		virtual int GetMaterialCount() override
		{
			return this->materialVec.size();
		};

		virtual void SetOutLineInfo(bool isOutLine, yunuGI::Color outLineColor) override 
		{
            if (lastOutlineRequest_isOutLine == isOutLine && (isOutLine == false || lastOutlineRequest_outLineColor == outLineColor))
            {
                return;
            }
            lastOutlineRequest_isOutLine = isOutLine;
            lastOutlineRequest_outLineColor = outLineColor;
			for (auto& each : renderable->renderInfoVec)
			{
				each->renderInfo.outlineInfo = DirectX::SimpleMath::Vector4{ outLineColor.r,outLineColor.g,outLineColor.b, renderable->GetID() };
				each->renderInfo.isOutLine = isOutLine;
			}
		};
	private:
		std::shared_ptr<SkinnedMesh> renderable;
		std::vector<std::shared_ptr<MaterialWrapper>> materialVec;
        bool lastOutlineRequest_isOutLine;
        yunuGI::Color lastOutlineRequest_outLineColor;
	};
}
