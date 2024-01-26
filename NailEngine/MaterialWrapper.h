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
		GetVariation()->SetPixelShader(shader);

		RenderSystem::Instance.Get().ReSortRenderInfo(this->renderable.get(), this->index);
	};

	virtual void SetTexture(yunuGI::Texture_Type textureType, const yunuGI::ITexture* texture) override
	{
		GetVariation()->SetTexture(textureType, texture);
	};

	virtual void SetInt(int index, int val) override
	{
		GetVariation()->SetInt(index, val);
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

	Material* GetMaterial()
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

	void SetRenderable(std::shared_ptr<IRenderable> renderable)
	{
		this->renderable = renderable;
	}

private:
	bool usingOriginal{ true };

	Material* GetVariation()
	{
		if (usingOriginal)
		{
			variation = ResourceManager::Instance.Get().CreateInstanceMaterial(original);
			usingOriginal = false;
			renderable->SetMaterial(0, variation);
		}

		return variation;
	};

public:
	Material* original;
	Material* variation;
	int index = 0;

private:
	std::shared_ptr<IRenderable> renderable;

};