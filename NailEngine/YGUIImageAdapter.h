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
            renderable->SetWorldTM(reinterpret_cast<const DirectX::SimpleMath::Matrix&>(worldTM));
            //renderable->pos.x = worldTM.m41;
            //renderable->pos.y = worldTM.m42;
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
        virtual float GetXPivot()override
        {
            return renderable->GetXPivot();
        }
        virtual float GetYPivot()override
        {
            return renderable->GetYPivot();
        }
        virtual void SetXPivot(float xPivot)override
        {
            renderable->SetXPivot(xPivot);
        }
        virtual void SetYPivot(float yPivot) override
        {
            renderable->SetYPivot(yPivot);
        }
        virtual int GetLayer() override
        {
            return renderable->GetLayer();
        }
        virtual yunuGI::Color GetColor()
        {
            auto color = renderable->GetColor();
            return reinterpret_cast<yunuGI::Color&>(color);
        };
        virtual void SetColor(const yunuGI::Color& color)
        {
            renderable->SetColor(reinterpret_cast<const DirectX::SimpleMath::Color&>(color));
        }
        virtual bool IsRadialFillMode()
        {
            return renderable->IsRadialFillMode();
        }
        // 텍스처가 중심에서부터 갉아먹는 형태로 채워지는지 여부를 설정
        virtual void SetRadialFillMode(bool fill)
        {
            renderable->SetRadialFillMode(fill);
        }
        virtual void SetRadialFillDegree(float degree)
        {
            renderable->SetRadialFillDegree(degree);
        }
        virtual void SetRadialFillStartPoint(float x, float y)
        {
            renderable->SetRadialFillStartPoint(x, y);
        }
        // 시계방향으로 채우는지 반시계 방향으로 채우는지 설정
        virtual void SetRadialFillDirection(bool isClockwise)
        {
            renderable->SetRadialFillDirection(isClockwise);
        }
    private:
        std::shared_ptr<UIImage> renderable;
    };
}
