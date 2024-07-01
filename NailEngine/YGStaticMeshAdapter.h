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

			std::shared_ptr<MaterialWrapper> material = std::make_shared<MaterialWrapper>(true, 0);
			material->SetRenderable(this->renderable);
			this->materialVec.emplace_back(material);

			/// here ok
			InstancingManager::Instance.Get().RegisterStaticDeferredData(static_cast<StaticMesh*>(renderable.get())->renderInfoVec[0]);
			//RenderSystem::Instance.Get().PushStaticRenderableObject(renderable.get());
		}

		~StaticMeshAdapter()
		{
			/// here ok
			for (int i = 0; i < static_cast<StaticMesh*>(renderable.get())->renderInfoVec.size(); ++i)
			{
				if (static_cast<StaticMesh*>(renderable.get())->renderInfoVec[i]->material->GetPixelShader()->GetShaderInfo().shaderType == yunuGI::ShaderType::Deferred)
				{
					InstancingManager::Instance.Get().PopStaticDeferredData(static_cast<StaticMesh*>(renderable.get())->renderInfoVec[i]);
				}
				else if(static_cast<StaticMesh*>(renderable.get())->renderInfoVec[i]->material->GetPixelShader()->GetShaderInfo().shaderType == yunuGI::ShaderType::Forward)
				{
					InstancingManager::Instance.Get().PopStaticForwardData(static_cast<StaticMesh*>(renderable.get())->renderInfoVec[i]);
				}
				else if (static_cast<StaticMesh*>(renderable.get())->renderInfoVec[i]->material->GetPixelShader()->GetShaderInfo().shaderType == yunuGI::ShaderType::Decal)
				{
					InstancingManager::Instance.Get().PopDecalData(static_cast<StaticMesh*>(renderable.get())->renderInfoVec[i]);
				}
			}
			//RenderSystem::Instance.Get().PopStaticRenderableObject(renderable.get());
		}

		virtual void SetWorldTM(const yunuGI::Matrix4x4& worldTM)
		{
			renderable->SetWorldTM(reinterpret_cast<const DirectX::SimpleMath::Matrix&>(worldTM));
			auto tempWTM = reinterpret_cast<const DirectX::SimpleMath::Matrix&>(worldTM);
			DirectX::SimpleMath::Vector3 pos;
			DirectX::SimpleMath::Vector3 scale;
			DirectX::SimpleMath::Quaternion quat;
			tempWTM.Decompose(scale, quat, pos);
			int minusCount = 0;
			if (scale.x < 0)
			{
				minusCount++;
			}
			if (scale.y < 0)
			{
				minusCount++;
			}
			if (scale.z < 0)
			{
				minusCount++;
			}
			

			if (minusCount % 2 == 1)
			{
				for (int i = 0; i < this->materialVec.size(); ++i)
				{
					auto material = this->GetMaterial(i);
					material->SetPixelShader(ResourceManager::Instance.Get().GetShader(L"Default_CullFrontPS.cso").get());
				}
			}
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
			if (isInstance)
			{
				return this->materialVec[index].get();
			}
			else
			{
				return this->materialVec[index].get()->GetMaterial();
			}
		};


	private:
		std::shared_ptr<StaticMesh> renderable;
		std::vector<std::shared_ptr<MaterialWrapper>> materialVec;
		
	};

}
