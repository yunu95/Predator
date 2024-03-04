#pragma once

#include "IRenderable.h"
#include "Struct.h"

#include "ResourceManager.h"
#include "InstancingManager.h"

#include <vector>
#include <memory>

class SkinnedMesh : public nail::IRenderable
{
public:
	SkinnedMesh()
	{
		this->isStatic = false;

		this->renderInfoVec.emplace_back(std::make_shared<SkinnedRenderInfo>());

		this->renderInfoVec.back()->renderInfo.mesh = this->mesh;
		this->renderInfoVec.back()->renderInfo.materialIndex = 0;
		this->renderInfoVec.back()->renderInfo.material = reinterpret_cast<Material*>(ResourceManager::Instance.Get().GetMaterial(L"SkinnedDefaultMaterial").get());
	}

	void SetBone(std::wstring fbxName)
	{
		this->fbxName = fbxName;
		for (int i = 0; i < renderInfoVec.size(); ++i)
		{
			renderInfoVec[i]->modelName = fbxName;
		}
	}

	const std::wstring& GetBone()
	{
		return this->fbxName;
	}

	void SetAnimatorIndex(unsigned int index)
	{
		this->animatorIndex = index;
		for (int i = 0; i < renderInfoVec.size(); ++i)
		{
			renderInfoVec[0]->animator = NailAnimatorManager::Instance.Get().GetAnimator(index);
		}
	}

	unsigned int GetAnimatorIndex()
	{
		return this->animatorIndex;
	}

////////////////////////////////////////////////////////////////////////////////// 
	virtual void SetWorldTM(const DirectX::SimpleMath::Matrix& wtm) override
	{
		this->wtm = wtm;
		for (auto& i : renderInfoVec)
		{
			i->renderInfo.wtm = wtm;
		}
	};

	virtual void SetMesh(Mesh* mesh) override
	{
		this->mesh = mesh;
		for (auto& i : renderInfoVec)
		{
			i->renderInfo.mesh = mesh;
			InstancingManager::Instance.Get().RegisterSkinnedData(i);
		}
	}

	virtual void SetMaterial(unsigned int index, yunuGI::IMaterial* material) override
	{
		// 새로운 Material이라면
		if (index + 1 > this->materialVec.size())
		{
			this->renderInfoVec.emplace_back(std::make_shared<SkinnedRenderInfo>());
			this->renderInfoVec.back()->renderInfo.mesh = this->mesh;

			this->renderInfoVec.back()->renderInfo.materialIndex = index;
			this->renderInfoVec.back()->renderInfo.material = reinterpret_cast<Material*>(material);
			this->renderInfoVec.back()->renderInfo.wtm = renderInfoVec[0]->renderInfo.wtm;
			this->renderInfoVec.back()->animator = NailAnimatorManager::Instance.Get().GetAnimator(this->animatorIndex);
			this->renderInfoVec.back()->modelName = this->fbxName;

			this->materialVec.emplace_back(reinterpret_cast<Material*>(material));

			RenderSystem::Instance.Get().RegisterSkinnedRenderInfo(this, this->renderInfoVec.back());
			InstancingManager::Instance.Get().RegisterSkinnedData(this->renderInfoVec.back());
		}
		else
		{
			if (this->materialVec[index] != reinterpret_cast<Material*>(material))
			{
				InstancingManager::Instance.Get().PopSkinnedData(renderInfoVec[index]);

				renderInfoVec[index]->renderInfo.mesh = this->mesh;
				renderInfoVec[index]->renderInfo.material = reinterpret_cast<Material*>(material);
				renderInfoVec[index]->renderInfo.wtm = renderInfoVec[index]->renderInfo.wtm;

				this->materialVec[index] = reinterpret_cast<Material*>(material);

				InstancingManager::Instance.Get().RegisterSkinnedData(renderInfoVec[index]);
			}
		}
	}

	virtual void SetActive(bool isActive) override
	{
		this->isActive = isActive;

		for (auto& i : this->renderInfoVec)
		{
			i->renderInfo.isActive = isActive;
		}
	}

public:
	std::vector<std::shared_ptr<SkinnedRenderInfo>> renderInfoVec;

private:
	std::wstring fbxName;
	unsigned int animatorIndex;
};

