#pragma once
#include "IMaterial.h"

#include "Resource.h"

#include "ResourceManager.h"
#include "InstancingManager.h"
#include "StaticMesh.h"
#include "SkinnedMesh.h"
#include "Material.h"

class MaterialWrapper : public yunuGI::IMaterial, public Resource
{
public:
	MaterialWrapper()
	{

	}
	MaterialWrapper(int index)
	{
		this->index = index;
	}
	MaterialWrapper(bool isStatic, int index)
	{
		if (isStatic)
		{
			this->original = ((Material*)(ResourceManager::Instance.Get().GetMaterial(L"DefaultMaterial").get()));
		}
		else
		{
			this->original = ((Material*)(ResourceManager::Instance.Get().GetMaterial(L"SkinnedDefaultMaterial").get()));
		}
		this->index = index;
	}
	~MaterialWrapper()
	{

	}

	virtual void SetName(const std::wstring& materialName) override
	{
		GetVariation()->SetName(materialName);
	};

	virtual void SetVertexShader(const yunuGI::IShader* shader) override
	{
		GetVariation()->SetVertexShader(shader);
	};

	virtual void SetPixelShader(const yunuGI::IShader* shader) override
	{
		// 스킨드메쉬라면
		if (!renderable->IsStatic())
		{
			if (static_cast<SkinnedMesh*>(renderable.get())->renderInfoVec[this->index]->renderInfo.material->GetPixelShader()->GetShaderInfo().shaderType == yunuGI::ShaderType::Deferred)
			{
				InstancingManager::Instance.Get().PopSkinnedDeferredData(static_cast<SkinnedMesh*>(renderable.get())->renderInfoVec[this->index]);
			}
			else
			{
				InstancingManager::Instance.Get().PopSkinnedForwardData(static_cast<SkinnedMesh*>(renderable.get())->renderInfoVec[this->index]);
			}
		}
		else
		{
			// 스태틱메쉬라면
			if (static_cast<StaticMesh*>(renderable.get())->renderInfoVec[this->index]->material->GetPixelShader()->GetShaderInfo().shaderType == yunuGI::ShaderType::Deferred)
			{
				InstancingManager::Instance.Get().PopStaticDeferredData(static_cast<StaticMesh*>(renderable.get())->renderInfoVec[this->index]);
			}
			else if(static_cast<StaticMesh*>(renderable.get())->renderInfoVec[this->index]->material->GetPixelShader()->GetShaderInfo().shaderType == yunuGI::ShaderType::Forward)
			{
				InstancingManager::Instance.Get().PopStaticForwardData(static_cast<StaticMesh*>(renderable.get())->renderInfoVec[this->index]);
			}
			else if (static_cast<StaticMesh*>(renderable.get())->renderInfoVec[this->index]->material->GetPixelShader()->GetShaderInfo().shaderType == yunuGI::ShaderType::Decal)
			{
				InstancingManager::Instance.Get().PopDecalData(static_cast<StaticMesh*>(renderable.get())->renderInfoVec[this->index]);
			}
		}

		// 픽셀 쉐이더를 세팅해주기전에 렌더링 방식이 바뀌었는지 검사한다.
		ChangeRenderType(shader);

		GetVariation()->SetPixelShader(shader);

		// 스태틱메쉬라면
		if (renderable->IsStatic())
		{
			/// here ok
			// 픽셀 쉐이더가 디퍼드,포워드,데칼인지에따른 분기문
			if (static_cast<StaticMesh*>(renderable.get())->renderInfoVec[this->index]->material->GetPixelShader()->GetShaderInfo().shaderType == yunuGI::ShaderType::Deferred)
			{
				InstancingManager::Instance.Get().RegisterStaticDeferredData(static_cast<StaticMesh*>(renderable.get())->renderInfoVec[this->index]);
			}
			else if (static_cast<StaticMesh*>(renderable.get())->renderInfoVec[this->index]->material->GetPixelShader()->GetShaderInfo().shaderType == yunuGI::ShaderType::Forward)
			{
				InstancingManager::Instance.Get().RegisterStaticForwardData(static_cast<StaticMesh*>(renderable.get())->renderInfoVec[this->index]);
			}
			else if (static_cast<StaticMesh*>(renderable.get())->renderInfoVec[this->index]->material->GetPixelShader()->GetShaderInfo().shaderType == yunuGI::ShaderType::Decal)
			{
				InstancingManager::Instance.Get().RegisterDecalData(static_cast<StaticMesh*>(renderable.get())->renderInfoVec[this->index]);
			}
		}
		else
		{
			// 스킨드메쉬라면
			if (static_cast<SkinnedMesh*>(renderable.get())->renderInfoVec[this->index]->renderInfo.material->GetPixelShader()->GetShaderInfo().shaderType == yunuGI::ShaderType::Deferred)
			{
				InstancingManager::Instance.Get().RegisterSkinnedDeferredData(static_cast<SkinnedMesh*>(renderable.get())->renderInfoVec[this->index]);
			}
			else
			{
				InstancingManager::Instance.Get().RegisterSkinnedForwardData(static_cast<SkinnedMesh*>(renderable.get())->renderInfoVec[this->index]);
			}
		}
	};

	virtual void SetTexture(yunuGI::Texture_Type textureType, const yunuGI::ITexture* texture) override
	{
		GetVariation()->SetTexture(textureType, texture);
	};

	virtual void SetInt(int index, int val) override
	{
		GetVariation()->SetInt(index, val);
	}

	virtual void SetFloat(int index, float val) override
	{
		GetVariation()->SetFloat(index, val);
	}

	virtual void SetColor(const yunuGI::Color& color) override
	{
		GetVariation()->SetColor(color);
	}

	virtual const std::wstring& GetName(bool isForceOrigin)const override
	{
		if (isForceOrigin)
		{
			return original->GetName();
		}
		else
		{
			if (usingOriginal)
				return original->GetName();
			else
				return variation->GetName();
		}
	}

	virtual const yunuGI::IShader* GetPixelShader() const override
	{
		if (usingOriginal)
			return original->GetPixelShader();
		else
			return variation->GetPixelShader();
	}

	virtual const yunuGI::IShader* GetVertexShader() const override
	{
		return nullptr;
	};
	virtual yunuGI::ITexture* GetTexture(yunuGI::Texture_Type textureType) override
	{
		return nullptr;
	};
	virtual yunuGI::IMaterial* GetMaterial() override
	{
		if (usingOriginal)
			return original;
		else
			return variation;
	}

	virtual yunuGI::Color& GetColor() override
	{
		if (usingOriginal)
			return original->GetColor();
		else
			return variation->GetColor();
	}

	bool IsOrigin()
	{
		return usingOriginal;
	}

	void SetIsOrigin(bool isOrigin)
	{
		usingOriginal = isOrigin;
		if (variation)
		{
			variation = nullptr;
		}
	}

	void SetRenderable(std::shared_ptr<nail::IRenderable> renderable)
	{
		this->renderable = renderable;
	}

	Material* GetVariation(bool isInstance = true)
	{
		if (isInstance)
		{
			if (usingOriginal)
			{
				yunuGI::IMaterial* tempMaterial = ResourceManager::Instance.Get().GetInstanceMaterial(original->GetName()).get();
				if (tempMaterial)
				{
					variation = static_cast<Material*>(tempMaterial);
				}
				else
				{
					variation = ResourceManager::Instance.Get().CreateInstanceMaterial(original);
				}

				renderable->SetMaterial(this->index, variation);
				usingOriginal = false;
			}

			return variation;
		}
		else
		{
			return original;
		}
	};
private:
	void ChangeRenderType(const yunuGI::IShader* shader)
	{
		if (GetVariation()->GetPixelShader()->GetShaderInfo().shaderType == yunuGI::ShaderType::Deferred)
		{
			if (shader->GetShaderInfo().shaderType == yunuGI::ShaderType::Forward)
			{
				if (!renderable->IsStatic())
				{
					if (static_cast<SkinnedMesh*>(renderable.get())->renderInfoVec[this->index]->renderInfo.material->GetPixelShader()->GetShaderInfo().shaderType == yunuGI::ShaderType::Deferred)
					{
						InstancingManager::Instance.Get().PopSkinnedDeferredData(static_cast<SkinnedMesh*>(renderable.get())->renderInfoVec[this->index]);
					}
					else
					{
						InstancingManager::Instance.Get().PopSkinnedForwardData(static_cast<SkinnedMesh*>(renderable.get())->renderInfoVec[this->index]);
					}
				}
				else
				{
					// 스태틱메쉬라면
					if (static_cast<StaticMesh*>(renderable.get())->renderInfoVec[this->index]->material->GetPixelShader()->GetShaderInfo().shaderType == yunuGI::ShaderType::Deferred)
					{
						InstancingManager::Instance.Get().PopStaticDeferredData(static_cast<StaticMesh*>(renderable.get())->renderInfoVec[this->index]);
					}
					else if (static_cast<StaticMesh*>(renderable.get())->renderInfoVec[this->index]->material->GetPixelShader()->GetShaderInfo().shaderType == yunuGI::ShaderType::Forward)
					{
						InstancingManager::Instance.Get().PopStaticForwardData(static_cast<StaticMesh*>(renderable.get())->renderInfoVec[this->index]);
					}
					else if (static_cast<StaticMesh*>(renderable.get())->renderInfoVec[this->index]->material->GetPixelShader()->GetShaderInfo().shaderType == yunuGI::ShaderType::Decal)
					{
						InstancingManager::Instance.Get().PopDecalData(static_cast<StaticMesh*>(renderable.get())->renderInfoVec[this->index]);
					}
				}
			}
			else if (shader->GetShaderInfo().shaderType == yunuGI::ShaderType::Decal)
			{
				if (!renderable->IsStatic())
				{
					if (static_cast<SkinnedMesh*>(renderable.get())->renderInfoVec[this->index]->renderInfo.material->GetPixelShader()->GetShaderInfo().shaderType == yunuGI::ShaderType::Deferred)
					{
						InstancingManager::Instance.Get().PopSkinnedDeferredData(static_cast<SkinnedMesh*>(renderable.get())->renderInfoVec[this->index]);
					}
					else
					{
						InstancingManager::Instance.Get().PopSkinnedForwardData(static_cast<SkinnedMesh*>(renderable.get())->renderInfoVec[this->index]);
					}
				}
				else
				{
					// 스태틱메쉬라면
					if (static_cast<StaticMesh*>(renderable.get())->renderInfoVec[this->index]->material->GetPixelShader()->GetShaderInfo().shaderType == yunuGI::ShaderType::Deferred)
					{
						InstancingManager::Instance.Get().PopStaticDeferredData(static_cast<StaticMesh*>(renderable.get())->renderInfoVec[this->index]);
					}
					else if (static_cast<StaticMesh*>(renderable.get())->renderInfoVec[this->index]->material->GetPixelShader()->GetShaderInfo().shaderType == yunuGI::ShaderType::Forward)
					{
						InstancingManager::Instance.Get().PopStaticForwardData(static_cast<StaticMesh*>(renderable.get())->renderInfoVec[this->index]);
					}
					else if (static_cast<StaticMesh*>(renderable.get())->renderInfoVec[this->index]->material->GetPixelShader()->GetShaderInfo().shaderType == yunuGI::ShaderType::Decal)
					{
						InstancingManager::Instance.Get().PopDecalData(static_cast<StaticMesh*>(renderable.get())->renderInfoVec[this->index]);
					}
				}
			}
		}
		else if(GetVariation()->GetPixelShader()->GetShaderInfo().shaderType == yunuGI::ShaderType::Forward)
		{
			if (shader->GetShaderInfo().shaderType == yunuGI::ShaderType::Deferred)
			{
				if (!renderable->IsStatic())
				{
					if (static_cast<SkinnedMesh*>(renderable.get())->renderInfoVec[this->index]->renderInfo.material->GetPixelShader()->GetShaderInfo().shaderType == yunuGI::ShaderType::Deferred)
					{
						InstancingManager::Instance.Get().PopSkinnedDeferredData(static_cast<SkinnedMesh*>(renderable.get())->renderInfoVec[this->index]);
					}
					else
					{
						InstancingManager::Instance.Get().PopSkinnedForwardData(static_cast<SkinnedMesh*>(renderable.get())->renderInfoVec[this->index]);
					}
				}
				else
				{
					// 스태틱메쉬라면
					if (static_cast<StaticMesh*>(renderable.get())->renderInfoVec[this->index]->material->GetPixelShader()->GetShaderInfo().shaderType == yunuGI::ShaderType::Deferred)
					{
						InstancingManager::Instance.Get().PopStaticDeferredData(static_cast<StaticMesh*>(renderable.get())->renderInfoVec[this->index]);
					}
					else if (static_cast<StaticMesh*>(renderable.get())->renderInfoVec[this->index]->material->GetPixelShader()->GetShaderInfo().shaderType == yunuGI::ShaderType::Forward)
					{
						InstancingManager::Instance.Get().PopStaticForwardData(static_cast<StaticMesh*>(renderable.get())->renderInfoVec[this->index]);
					}
					else if (static_cast<StaticMesh*>(renderable.get())->renderInfoVec[this->index]->material->GetPixelShader()->GetShaderInfo().shaderType == yunuGI::ShaderType::Decal)
					{
						InstancingManager::Instance.Get().PopDecalData(static_cast<StaticMesh*>(renderable.get())->renderInfoVec[this->index]);
					}
				}
			}
			else if (shader->GetShaderInfo().shaderType == yunuGI::ShaderType::Decal)
			{
				if (!renderable->IsStatic())
				{
					if (static_cast<SkinnedMesh*>(renderable.get())->renderInfoVec[this->index]->renderInfo.material->GetPixelShader()->GetShaderInfo().shaderType == yunuGI::ShaderType::Deferred)
					{
						InstancingManager::Instance.Get().PopSkinnedDeferredData(static_cast<SkinnedMesh*>(renderable.get())->renderInfoVec[this->index]);
					}
					else
					{
						InstancingManager::Instance.Get().PopSkinnedForwardData(static_cast<SkinnedMesh*>(renderable.get())->renderInfoVec[this->index]);
					}
				}
				else
				{
					// 스태틱메쉬라면
					if (static_cast<StaticMesh*>(renderable.get())->renderInfoVec[this->index]->material->GetPixelShader()->GetShaderInfo().shaderType == yunuGI::ShaderType::Deferred)
					{
						InstancingManager::Instance.Get().PopStaticDeferredData(static_cast<StaticMesh*>(renderable.get())->renderInfoVec[this->index]);
					}
					else if (static_cast<StaticMesh*>(renderable.get())->renderInfoVec[this->index]->material->GetPixelShader()->GetShaderInfo().shaderType == yunuGI::ShaderType::Forward)
					{
						InstancingManager::Instance.Get().PopStaticForwardData(static_cast<StaticMesh*>(renderable.get())->renderInfoVec[this->index]);
					}
					else if (static_cast<StaticMesh*>(renderable.get())->renderInfoVec[this->index]->material->GetPixelShader()->GetShaderInfo().shaderType == yunuGI::ShaderType::Decal)
					{
						InstancingManager::Instance.Get().PopDecalData(static_cast<StaticMesh*>(renderable.get())->renderInfoVec[this->index]);
					}
				}
			}
		}
		else if (GetVariation()->GetPixelShader()->GetShaderInfo().shaderType == yunuGI::ShaderType::Decal)
		{
			if (shader->GetShaderInfo().shaderType == yunuGI::ShaderType::Deferred)
			{
				if (!renderable->IsStatic())
				{
					if (static_cast<SkinnedMesh*>(renderable.get())->renderInfoVec[this->index]->renderInfo.material->GetPixelShader()->GetShaderInfo().shaderType == yunuGI::ShaderType::Deferred)
					{
						InstancingManager::Instance.Get().PopSkinnedDeferredData(static_cast<SkinnedMesh*>(renderable.get())->renderInfoVec[this->index]);
					}
					else
					{
						InstancingManager::Instance.Get().PopSkinnedForwardData(static_cast<SkinnedMesh*>(renderable.get())->renderInfoVec[this->index]);
					}
				}
				else
				{
					// 스태틱메쉬라면
					if (static_cast<StaticMesh*>(renderable.get())->renderInfoVec[this->index]->material->GetPixelShader()->GetShaderInfo().shaderType == yunuGI::ShaderType::Deferred)
					{
						InstancingManager::Instance.Get().PopStaticDeferredData(static_cast<StaticMesh*>(renderable.get())->renderInfoVec[this->index]);
					}
					else if (static_cast<StaticMesh*>(renderable.get())->renderInfoVec[this->index]->material->GetPixelShader()->GetShaderInfo().shaderType == yunuGI::ShaderType::Forward)
					{
						InstancingManager::Instance.Get().PopStaticForwardData(static_cast<StaticMesh*>(renderable.get())->renderInfoVec[this->index]);
					}
					else if (static_cast<StaticMesh*>(renderable.get())->renderInfoVec[this->index]->material->GetPixelShader()->GetShaderInfo().shaderType == yunuGI::ShaderType::Decal)
					{
						InstancingManager::Instance.Get().PopDecalData(static_cast<StaticMesh*>(renderable.get())->renderInfoVec[this->index]);
					}
				}
			}
			else if (shader->GetShaderInfo().shaderType == yunuGI::ShaderType::Forward)
			{
				if (!renderable->IsStatic())
				{
					if (static_cast<SkinnedMesh*>(renderable.get())->renderInfoVec[this->index]->renderInfo.material->GetPixelShader()->GetShaderInfo().shaderType == yunuGI::ShaderType::Deferred)
					{
						InstancingManager::Instance.Get().PopSkinnedDeferredData(static_cast<SkinnedMesh*>(renderable.get())->renderInfoVec[this->index]);
					}
					else
					{
						InstancingManager::Instance.Get().PopSkinnedForwardData(static_cast<SkinnedMesh*>(renderable.get())->renderInfoVec[this->index]);
					}
				}
				else
				{
					// 스태틱메쉬라면
					if (static_cast<StaticMesh*>(renderable.get())->renderInfoVec[this->index]->material->GetPixelShader()->GetShaderInfo().shaderType == yunuGI::ShaderType::Deferred)
					{
						InstancingManager::Instance.Get().PopStaticDeferredData(static_cast<StaticMesh*>(renderable.get())->renderInfoVec[this->index]);
					}
					else if (static_cast<StaticMesh*>(renderable.get())->renderInfoVec[this->index]->material->GetPixelShader()->GetShaderInfo().shaderType == yunuGI::ShaderType::Forward)
					{
						InstancingManager::Instance.Get().PopStaticForwardData(static_cast<StaticMesh*>(renderable.get())->renderInfoVec[this->index]);
					}
					else if (static_cast<StaticMesh*>(renderable.get())->renderInfoVec[this->index]->material->GetPixelShader()->GetShaderInfo().shaderType == yunuGI::ShaderType::Decal)
					{
						InstancingManager::Instance.Get().PopDecalData(static_cast<StaticMesh*>(renderable.get())->renderInfoVec[this->index]);
					}
				}
			}
		}
	}
	bool usingOriginal{ true };



public:
	Material* original;
	Material* variation;
	int index = 0;

private:
	std::shared_ptr<nail::IRenderable> renderable;

};