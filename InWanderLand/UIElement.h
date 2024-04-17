#pragma once
#include "YunutyEngine.h"
#include "JsonUIData.h"
#include "UIExportFlag.h"
#include "UIOffsetTransition.h"

class UIButton;
class UIImage;
// 임포트된 UI 요소에 대한 정보를 잔뜩 저장하는 클래스
class UIElement : public Component
{
private:
    virtual void Start() override
    {
        application::contents::ContentsLayer* contentsLayer = dynamic_cast<application::contents::ContentsLayer*>(application::Application::GetInstance().GetContentsLayer());
        // 게임이 끝나면 삭제되도록 설정
        if (GetGameObject()->GetParentGameObject() == nullptr)
        {
            contentsLayer->RegisterToEditorObjectContainer(GetGameObject());
        }
    }
    JsonUIData importedUIData;
    PopupOnEnable* scalePopUpTransition{ nullptr };
    UIOffsetTransition* enableTransition{ nullptr };
    UIOffsetTransition* disableTransition{ nullptr };
    UIImage* imageComponent{ nullptr };
    UIButton* button{ nullptr };
public:
    void EnableElement()
    {
        GetGameObject()->SetSelfActive(true);
        if (scalePopUpTransition != nullptr)
        {
            scalePopUpTransition->ActivateTimer();
        }
        if (enableTransition != nullptr)
        {
            enableTransition->ActivateTimer();
        }
    }
    void DisableElement()
    {
        bool disablingHandled = false;
        if (disableTransition != nullptr)
        {
            disablingHandled = true;
            disableTransition->ActivateTimer();
        }
        if (disablingHandled == false)
        {
            GetGameObject()->SetSelfActive(false);
        }
    }
    friend class UIManager;
};
