#pragma once
#include <unordered_map>
#include <DirectXMath.h>
#include "YunuGraphicsInterface.h"
#include "YGRenderableAdapter.h"

#include "RenderableManager.h"
#include "UIImage.h"

namespace yunuGIAdapter
{
    class UIImageAdapter : public yunuGIAdapter::RenderableAdapter, public yunuGI::IUIImage
    {
    public:
        UIImageAdapter() :RenderableAdapter() 
        {
			renderable = std::make_shared<UIImage>();
			RenderableManager::Instance.Get().PushUIObject(renderable);
            RenderableManager::Instance.Get().ReSortUIObject(renderable->layer, renderable);
        }

		~UIImageAdapter()
		{
			RenderableManager::Instance.Get().PopUIObject(renderable);
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

        virtual void SetImage(yunuGI::ITexture* texture)
        {
            renderable->SetTexture(texture);
        };

        virtual void SetScreenSpace() {}
        virtual void SetWorldSpace() {}
        virtual void SetPickingMode(bool isPickingModeOn) {}

		virtual void SetLayer(int layer)
		{
            renderable->layer = layer;
            RenderableManager::Instance.Get().ReSortUIObject(layer, renderable);
		};

    private:
        std::shared_ptr<UIImage> renderable;
    };
}
