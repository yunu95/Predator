#pragma once
#include "IMaterial.h"

#include "Resource.h"

#include "ResourceManager.h"
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
		if (renderable->IsStatic() == false)
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

		GetVariation()->SetPixelShader(shader);

		if (renderable->IsStatic())
		{
			RenderSystem::Instance.Get().ReSortRenderInfo(renderable.get(), this->index);
		}
		else
		{
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

	virtual const std::wstring& GetName()const override
	{
		if (usingOriginal)
			return original->GetName();
		else
			return variation->GetName();
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
				if (original->GetPixelShader()->GetShaderInfo().shaderType == yunuGI::ShaderType::Deferred)
				{
					if (renderable->IsStatic())
					{
						InstancingManager::Instance.Get().PopStaticDeferredData(std::static_pointer_cast<StaticMesh>(renderable)->renderInfoVec[this->index]);
					}
				}
				else
				{
					if (renderable->IsStatic())
					{
						InstancingManager::Instance.Get().PopStaticForwardData(std::static_pointer_cast<StaticMesh>(renderable)->renderInfoVec[this->index]);
					}
				}

				yunuGI::IMaterial* tempMaterial = ResourceManager::Instance.Get().GetInstanceMaterial(original->GetName()).get();
				if (tempMaterial)
				{
					variation = static_cast<Material*>(tempMaterial);
				}
				else
				{

					if (renderable->IsStatic() == false)
					{
						InstancingManager::Instance.Get().PopSkinnedDeferredData(std::static_pointer_cast<SkinnedMesh>(renderable)->renderInfoVec[this->index]);
					}

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
	bool usingOriginal{ true };



public:
	Material* original;
	Material* variation;
	int index = 0;

private:
	std::shared_ptr<nail::IRenderable> renderable;

};