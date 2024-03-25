#pragma once
#include <unordered_map>
#include <DirectXMath.h>
#include "YunuGraphicsInterface.h"
#include "YGRenderableAdapter.h"

#include "RenderSystem.h"
#include "UIText.h"

namespace yunuGIAdapter
{
    class UITextAdapter : public yunuGIAdapter::RenderableAdapter, public yunuGI::IUIText
    {
    public:
        UITextAdapter() :RenderableAdapter()
        { 
            renderable = std::make_shared<UIText>();
            RenderSystem::Instance.Get().PushTextObject(renderable);
        }

        ~UITextAdapter()
        {
            RenderSystem::Instance.Get().PopTextObject(renderable);
        }

        virtual void SetWorldTM(const yunuGI::Matrix4x4& worldTM)
		{
			renderable->pos.x = worldTM.m41;
			renderable->pos.y = worldTM.m42;

			renderable->scale.x = worldTM.m11;
			renderable->scale.y = worldTM.m22;
        };

        virtual void SetActive(bool isActive) 
        { 
            renderable->SetActive(isActive);
        };

        virtual void SetText(std::wstring str) override 
        {
            renderable->text = str;
        };

        virtual const std::wstring& GetText() override 
        {
            return  renderable->text;
        };

        virtual void SetPickingMode(bool isPickingModeOn) 
        {}

        virtual void SetFontSize(int size)
        {
            renderable->SetFontSize(size);
        };

        virtual void SetFont(std::wstring font)
        {
            renderable->SetFont(font);
        };

        virtual void SetColor(yunuGI::Color color)
        {
            renderable->SetColor(color);
        };


		virtual bool IsActive() override
		{
            return true;
		}
    private:
        std::shared_ptr<UIText> renderable;
    };
}
