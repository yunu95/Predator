#pragma once
#include <unordered_map>
#include <DirectXMath.h>
#include "YunuGraphicsInterface.h"
#include "YGRenderableAdapter.h"

#include "RenderSystem.h"
#include "UIImage.h"

namespace yunuGIAdapter
{
    class UIImageAdapter : public yunuGIAdapter::RenderableAdapter, public yunuGI::IUIImage
    {
    public:
        UIImageAdapter() :RenderableAdapter()
        {
            renderable = std::make_shared<UIImage>();
            RenderSystem::Instance.Get().PushUIObject(renderable);
            RenderSystem::Instance.Get().ReSortUIObject(std::static_pointer_cast<UIImage>(renderable)->layer, renderable);
        }

        ~UIImageAdapter()
        {
            RenderSystem::Instance.Get().PopUIObject(renderable);
        }

        virtual void SetWorldTM(const yunuGI::Matrix4x4& worldTM)
        {
            renderable->pos.x = worldTM.m41;
            renderable->pos.y = worldTM.m42;
        };

        virtual void SetActive(bool isActive)
        {
            renderable->SetActive(isActive);
        };
        virtual bool IsActive() override
        {
            return true;
        }

        virtual void SetImage(yunuGI::ITexture* texture)
        {
            renderable->SetTexture(texture);
        };

        virtual void SetScreenSpace() {}
        virtual void SetWorldSpace() {}
        virtual void SetPickingMode(bool isPickingModeOn) {}

        virtual void SetLayer(int layer)
        {
            //renderable->layer = layer;
            RenderSystem::Instance.Get().ReSortUIObject(layer, renderable);
        };

        virtual float GetWidth() override
        {
            return renderable->GetWidth();
        };
        virtual float GetHeight() override
        {
            return renderable->GetHeight();
        };
        virtual void SetWidth(float width)
        {
            renderable->SetWidth(width);
        };
        virtual void SetHeight(float height)
        {
            renderable->SetHeight(height);
        };
        virtual int GetLayer() override
        {
            return renderable->GetLayer();
        }

    private:
        std::shared_ptr<UIImage> renderable;
    };
}
