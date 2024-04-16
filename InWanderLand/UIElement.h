#pragma once
#include "YunutyEngine.h"

class UIButton;
class UIImage;
// 임포트된 UI 요소에 대한 정보를 잔뜩 저장하는 클래스
class UIElement : public Component
{
private:
    virtual void Start() override
    {
        application::contents::ContentsLayer* contentsLayer = dynamic_cast<application::contents::ContentsLayer*>(application::Application::GetInstance().GetContentsLayer());
        if (GetGameObject()->GetParentGameObject() == nullptr)
        {
            contentsLayer->RegisterToEditorObjectContainer(GetGameObject());
        }
    }
public:
    UIImage* imageComponent{ nullptr };
    UIButton* button{ nullptr };
};
