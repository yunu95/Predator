#pragma once
#include "IRenderable.h"

#include "Texture.h"

class UIImage : public nail::IRenderable
{
public:
	void SetTexture(yunuGI::ITexture* texture)
	{
		this->texture = texture;
	}

	yunuGI::ITexture* GetTexture()
	{
		return this->texture;
	}

	float GetWidth()
	{
		return (texture)->GetWidth();
	};

	float GetHeight()
	{
		return (texture)->GetHeight();
	};

	bool operator<(const UIImage& other) const
	{
		return layer < other.layer;
	}

public:
	DirectX::SimpleMath::Vector2 pos;
	int layer = 0;

private:
	yunuGI::ITexture* texture;
};

struct CompareSmartPtr
{
	bool operator()(const std::shared_ptr<nail::IRenderable>& lhs, const std::shared_ptr<nail::IRenderable>& rhs) const
	{
		const auto& leftImage = std::static_pointer_cast<UIImage>(lhs);
		const auto& rightImage = std::static_pointer_cast<UIImage>(rhs);

		if (leftImage->layer != rightImage->layer)
		{
			return leftImage->layer < rightImage->layer; // 다른 layer인 경우에만 비교
		}
		else
		{
			return *leftImage < *rightImage;
		}
	}
};