#pragma once
#include "IRenderable.h"

#include "Texture.h"

class UIImage : public nail::IRenderable
{
public:
    void SetTexture(yunuGI::ITexture* texture)
    {
        this->texture = texture;
        if (width == -1 && height == -1)
        {
            SetWidth(texture->GetWidth());
            SetHeight(texture->GetHeight());
        }
    }

    yunuGI::ITexture* GetTexture()
    {
        return this->texture;
    }

    float GetWidth()
    {
        return width;
    };

    float GetHeight()
    {
        return height;
    };
    void SetWidth(float width)
    {
        this->width = width;
    }
    void SetHeight(float height)
    {
        this->height = height;
    };
    bool operator<(const UIImage& other) const
    {
        return layer < other.layer;
    }

    int GetLayer()
    {
        return layer;
    }

public:
    DirectX::SimpleMath::Vector2 pos;
    int layer = 0;

private:
    float width{ -1 };
    float height{ -1 };
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
            return leftImage->layer < rightImage->layer;
        }
        else
        {
            return *leftImage < *rightImage;
        }
    }
};