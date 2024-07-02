#pragma once
#include "IRenderable.h"
#include "Struct.h"

#include "ResourceManager.h"
#include "InstancingManager.h"
#include "FrustumCullingManager.h"

#include <vector>
#include <memory>

class StaticMesh : public nail::IRenderable
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

	void UpdateQuadTree(DirectX::SimpleMath::Vector3 pos, DirectX::SimpleMath::Vector3 scale, DirectX::SimpleMath::Quaternion quat)
	{

	}

	virtual void SetWorldTM(const DirectX::SimpleMath::Matrix& wtm) override
	{
		DirectX::SimpleMath::Vector3 pos;
		DirectX::SimpleMath::Quaternion quat;
		DirectX::SimpleMath::Vector3 scale;

		auto& quadTree = InstancingManager::Instance.Get().GetQuadTree();
		this->wtm = wtm;
		for (auto& i : renderInfoVec)
		{
			i->wtm.Decompose(scale, quat, pos);

			i->wtm = wtm;

			if ((i->mesh))
			{
				if ((quadTree.GetDataSize() > 0))
				{
					if ((InstancingManager::Instance.Get().IsInTree(i)))
					{
						auto radius = i->mesh->GetBoundingRadius();

						float maxValue = std::max(std::max(scale.x, scale.y), scale.z);
						radius *= maxValue;

						quadTree.UpdatePosition(DirectX::SimpleMath::Vector2{ pos.x, pos.z }, radius, i.get());
					}
				}
			}
		}
	};
	virtual void SetMesh(Mesh* mesh) override
	{
		this->mesh = mesh;

		for (auto& i : renderInfoVec)
		{
			for (auto& each : this->materialVec)
			{
				// 메쉬를 갱신하기 전에 이전 메쉬에 대한 정보를 뺀다.
				if (each->GetPixelShader()->GetShaderInfo().shaderType == yunuGI::ShaderType::Deferred)
				{
					InstancingManager::Instance.Get().PopStaticDeferredData(i);
				}
				else if(each->GetPixelShader()->GetShaderInfo().shaderType == yunuGI::ShaderType::Forward)
				{
					InstancingManager::Instance.Get().PopStaticForwardData(i);
				}
				else if (each->GetPixelShader()->GetShaderInfo().shaderType == yunuGI::ShaderType::Decal)
				{
					InstancingManager::Instance.Get().PopDecalData(i);
				}

				i->mesh = mesh;

				// 메쉬가 갱신되었으니 새로 등록해준다.
				if (each->GetPixelShader()->GetShaderInfo().shaderType == yunuGI::ShaderType::Deferred)
				{
					InstancingManager::Instance.Get().RegisterStaticDeferredData(i);
				}
				else if (each->GetPixelShader()->GetShaderInfo().shaderType == yunuGI::ShaderType::Forward)
				{
					InstancingManager::Instance.Get().RegisterStaticForwardData(i);
				}
				else if (each->GetPixelShader()->GetShaderInfo().shaderType == yunuGI::ShaderType::Decal)
				{
					InstancingManager::Instance.Get().RegisterDecalData(i);
				}
			}
			//}
		}
	}
	yunuGI::IMesh* GetMesh()
	{
		return this->mesh;
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
			this->renderInfoVec.back()->wtm = this->renderInfoVec[0]->wtm;

			this->materialVec.emplace_back(reinterpret_cast<Material*>(material));

			/// here ok
			if (material->GetPixelShader()->GetShaderInfo().shaderType == yunuGI::ShaderType::Deferred)
			{
				InstancingManager::Instance.Get().RegisterStaticDeferredData(this->renderInfoVec.back());
			}
			else if (material->GetPixelShader()->GetShaderInfo().shaderType == yunuGI::ShaderType::Forward)
			{
				InstancingManager::Instance.Get().RegisterStaticForwardData(this->renderInfoVec.back());
			}
			else if (material->GetPixelShader()->GetShaderInfo().shaderType == yunuGI::ShaderType::Decal)
			{
				InstancingManager::Instance.Get().RegisterDecalData(this->renderInfoVec.back());
			}
			//RenderSystem::Instance.Get().RegisterRenderInfo(this, this->renderInfoVec.back());
			//RenderSystem::Instance.Get().ReSortRenderInfo(this, index);
		}
		else
		{
			if (this->materialVec[index] != reinterpret_cast<Material*>(material))
			{
				if (this->materialVec[index]->GetPixelShader()->GetShaderInfo().shaderType == yunuGI::ShaderType::Deferred)
				{
					InstancingManager::Instance.Get().PopStaticDeferredData(renderInfoVec[index]);
				}
				else if (this->materialVec[index]->GetPixelShader()->GetShaderInfo().shaderType == yunuGI::ShaderType::Forward)
				{
					InstancingManager::Instance.Get().PopStaticForwardData(renderInfoVec[index]);
				}
				else if (this->materialVec[index]->GetPixelShader()->GetShaderInfo().shaderType == yunuGI::ShaderType::Decal)
				{
					InstancingManager::Instance.Get().PopDecalData(renderInfoVec[index]);
				}

				renderInfoVec[index]->mesh = this->mesh;
				renderInfoVec[index]->material = reinterpret_cast<Material*>(material);
				renderInfoVec[index]->wtm = renderInfoVec[index]->wtm;

				this->materialVec[index] = reinterpret_cast<Material*>(material);

				if (reinterpret_cast<Material*>(material)->GetPixelShader()->GetShaderInfo().shaderType == yunuGI::ShaderType::Deferred)
				{
					InstancingManager::Instance.Get().RegisterStaticDeferredData(renderInfoVec[index]);
				}
				else if (reinterpret_cast<Material*>(material)->GetPixelShader()->GetShaderInfo().shaderType == yunuGI::ShaderType::Forward)
				{
					InstancingManager::Instance.Get().RegisterStaticForwardData(renderInfoVec[index]);
				}
				else if (reinterpret_cast<Material*>(material)->GetPixelShader()->GetShaderInfo().shaderType == yunuGI::ShaderType::Decal)
				{
					InstancingManager::Instance.Get().RegisterDecalData(renderInfoVec[index]);
				}
			}

			/// here ok
			//RenderSystem::Instance.Get().RegisterRenderInfo(this, this->renderInfoVec.back());
			//RenderSystem::Instance.Get().ReSortRenderInfo(this, index);
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

	void SetLightMapUVOffset(float x, float y)
	{
		DirectX::SimpleMath::Vector2 offset{ x,y };
		for (auto& i : renderInfoVec)
		{
			i->uvOffset = offset;
		}
	};

	void SetLightMapUVScaling(float x, float y)
	{
		DirectX::SimpleMath::Vector2 scale{ x,y };
		for (auto& i : renderInfoVec)
		{
			i->uvScaling = scale;
		}
	};

	void SetLightMapUVIndex(int index)
	{
		for (auto& i : renderInfoVec)
		{
			i->lightMapIndex = index;
		}
	};

public:
	std::vector<std::shared_ptr<RenderInfo>> renderInfoVec;
};

