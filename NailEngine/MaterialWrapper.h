#pragma once
#include "YGMaterialAdapter.h"
#include "ResourceManager.h"

class MaterialWrapper : public yunuGI::IMaterial
{
public:
	MaterialWrapper()
	{
		this->original = ResourceManager::Instance.Get().GetMaterial(L"DefaultMaterial").get();
	}

	virtual void SetMaterialName(const std::wstring& materialName) override
	{
		GetVariation()->SetMaterialName(materialName);
	};

	virtual void SetVertexShader(const std::wstring& vertexShader) override
	{
		GetVariation()->SetVertexShader(vertexShader);
	};

	virtual void SetPixelShader(const std::wstring& pixelShader) override
	{
		GetVariation()->SetPixelShader(pixelShader);
	};

	virtual void SetTexture(yunuGI::Texture_Type textureType, const std::wstring& texture) override
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

	virtual const std::wstring& GetMaterialName()const override
	{
		if (usingOriginal)
			return original->GetMaterialName();
		else
			return variation->GetMaterialName();
	}

	yunuGIAdapter::MaterialAdapter* GetMaterial()
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

	void SetRenderable(std::shared_ptr<StaticMesh>& renderable)
	{
		this->renderable = renderable;
	}

private:
	bool usingOriginal{ true };
	yunuGIAdapter::MaterialAdapter* GetVariation()
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
	yunuGIAdapter::MaterialAdapter* original;
	yunuGIAdapter::MaterialAdapter* variation;

private:
	std::shared_ptr<StaticMesh> renderable;

};