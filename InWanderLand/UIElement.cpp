#include "Application.h"
#include "ContentsLayer.h"
#include "UIElement.h"
void UIElement::Start()
{
    application::contents::ContentsLayer* contentsLayer = dynamic_cast<application::contents::ContentsLayer*>(application::Application::GetInstance().GetContentsLayer());
    // 게임이 끝나면 삭제되도록 설정
    if (GetGameObject()->GetParentGameObject() == nullptr)
    {
        contentsLayer->RegisterToEditorObjectContainer(GetGameObject());
    }
};
