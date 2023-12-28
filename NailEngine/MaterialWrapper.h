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
	MaterialWrapper(bool isStatic)
	{
		if (isStatic)
		{
			this->original = std::static_pointer_cast<Material>(ResourceManager::Instance.Get().GetMaterial(L"DefaultMaterial"));
		}
		else
		{
			this->original = std::static_pointer_cast<Material>(ResourceManager::Instance.Get().GetMaterial(L"SkinnedDefaultMaterial"));
		}
	}
	~MaterialWrapper()
	{
		int a = 1;
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
			return original.get();
		else
			return variation.get();
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

	std::shared_ptr<Material> GetVariation()
	{
		if (usingOriginal)
		{
			variation = ResourceManager::Instance.Get().CreateInstanceMaterial(original);
			usingOriginal = false;
			renderable->SetMaterial(0, variation.get());
		}

		return variation;
	};

public:
	std::shared_ptr<Material> original;
	std::shared_ptr<Material> variation;

private:
	std::shared_ptr<IRenderable> renderable;

};