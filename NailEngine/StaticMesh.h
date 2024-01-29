#pragma once
#include "IRenderable.h"
#include "Struct.h"

#include "ResourceManager.h"
#include "InstancingManager.h"

#include <vector>
#include <memory>

class StaticMesh : public IRenderable
{
public:
	StaticMesh()
	{
		this->isStatic = true;

		this->renderInfoVec.emplace_back(std::make_shared<RenderInfo>());
		
		this->renderInfoVec.back()->mesh = this->mesh;
		this->renderInfoVec.back()->materialIndex = 0;
		this->renderInfoVec.back()->material = reinterpret_cast<Material*>(ResourceManager::Instance.Get().GetMaterial(L"DefaultMaterial").get());
	}

	~StaticMesh()
	{

	}
	virtual void SetWorldTM(const DirectX::SimpleMath::Matrix& wtm) override
	{
		this->wtm = wtm;
		for (auto& i : renderInfoVec)
		{
			i->wtm = wtm;
		}
	};
	virtual void SetMesh(Mesh* mesh) override
	{
		this->mesh = mesh;
		for (auto& i : renderInfoVec)
		{
			i->mesh = mesh;
			InstancingManager::Instance.Get().RegisterStaticDeferredData(i);
		}
	}
	virtual void SetMaterial(unsigned int index, yunuGI::IMaterial* material) override
	{
		// 새로운 Material이라면
		if (index + 1 > this->materialVec.size())
		{
			this->renderInfoVec.emplace_back(std::make_shared<RenderInfo>());
			this->renderInfoVec.back()->mesh = this->mesh;

			this->renderInfoVec.back()->materialIndex = index;
			this->renderInfoVec.back()->material = reinterpret_cast<Material*>(material);
			this->materialVec.emplace_back(reinterpret_cast<Material*>(material));


			RenderSystem::Instance.Get().RegisterRenderInfo(this, this->renderInfoVec.back());
			RenderSystem::Instance.Get().ReSortRenderInfo(this, index);
		}
		else
		{
			if (this->materialVec[index] != reinterpret_cast<Material*>(material))
			{
				//if (this->materialVec[index]->GetPixelShader()->GetShaderInfo().shaderType == yunuGI::ShaderType::Deferred)
				//{
				//	InstancingManager::Instance.Get().PopStaticDeferredData(renderInfoVec[index]);
				//}
				//else
				//{
				//	InstancingManager::Instance.Get().PopStaticForwardData(renderInfoVec[index]);
				//}

				renderInfoVec[index]->mesh = this->mesh;
				renderInfoVec[index]->material = reinterpret_cast<Material*>(material);

				this->materialVec[index] = reinterpret_cast<Material*>(material);

				if (reinterpret_cast<Material*>(material)->GetPixelShader()->GetShaderInfo().shaderType == yunuGI::ShaderType::Deferred)
				{
					InstancingManager::Instance.Get().RegisterStaticDeferredData(renderInfoVec[index]);
				}
				else
				{
					InstancingManager::Instance.Get().RegisterStaticForwardData(renderInfoVec[index]);
				}
			}
		}
	}

	virtual void SetActive(bool isActive) override
	{
		this->isActive = isActive;

		for (auto& i : this->renderInfoVec)
		{
			i->isActive = isActive;
		}
	}

public:
	std::vector<std::shared_ptr<RenderInfo>> renderInfoVec;
};

