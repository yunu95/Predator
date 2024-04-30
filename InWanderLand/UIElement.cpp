#include "InWanderLand.h"

void UIElement::Start()
{
    application::contents::ContentsLayer* contentsLayer = dynamic_cast<application::contents::ContentsLayer*>(application::Application::GetInstance().GetContentsLayer());
    // 게임이 끝나면 삭제되도록 설정
    if (GetGameObject()->GetParentGameObject() == nullptr)
    {
        //contentsLayer->RegisterToEditorObjectContainer(GetGameObject());
    }
    // 만약 숫자 UI라면 먼저 숫자들을 모두 비활성화
    for (auto each : digits)
    {
        each->GetGameObject()->SetSelfActive(false);
    }
};
void UIElement::EnableElement()
{
    GetGameObject()->SetSelfActive(true);
    if (scalePopDownTransition)
        scalePopDownTransition->StopTimer();
    if (soundOnDisable)
        soundOnDisable->StopTimer();
    if (disableTransition)
        disableTransition->StopTimer();
    if (scalePopUpTransition != nullptr)
    {
        scalePopUpTransition->ActivateTimer();
    }
    if (enableTransition != nullptr)
    {
        enableTransition->ActivateTimer();
    }
    if (soundOnEnable != nullptr)
    {
        soundOnEnable->ActivateTimer();
    }
    if (parentPriorityLayout)
    {
        parentPriorityLayout->EnableChildUI(GetGameObject());
    }
    for (auto each : children)
    {
        if (each->GetGameObject()->GetSelfActive())
        {
            each->EnableElement();
        }
    }
}
void UIElement::DisableElement()
{
    bool disablingHandled = false;
    if (scalePopUpTransition)
        scalePopUpTransition->StopTimer();
    if (enableTransition)
        enableTransition->StopTimer();
    if (soundOnEnable)
        soundOnEnable->StopTimer();
    if (scalePopDownTransition != nullptr)
    {
        disablingHandled = true;
        scalePopDownTransition->ActivateTimer();
    }
    if (disableTransition != nullptr)
    {
        disablingHandled = true;
        disableTransition->ActivateTimer();
    }
    if (soundOnDisable != nullptr)
    {
        soundOnDisable->ActivateTimer();
    }
    if (disablingHandled == false)
    {
        GetGameObject()->SetSelfActive(false);
    }
    if (parentPriorityLayout)
    {
        parentPriorityLayout->DisableChildUI(GetGameObject());
    }
}
void UIElement::SetNumber(int number)
{
    assert(digitFont);
    int maxDigit = log10(number);
    assert(digits.size() > log10(number));
    if (number > 0)
    {
        for (int i = 0; i < digits.size(); i++)
        {
            if (i > maxDigit)
            {
                digits[i]->DisableElement();
            }
            else
            {
                digits[i]->EnableElement();
                digits[i]->imageComponent->GetGI().SetImage((*digitFont)[number % 10]);
                number /= 10;
            }
        }
    }
    else
    {
        digits[0]->EnableElement();
        digits[0]->imageComponent->GetGI().SetImage((*digitFont)[0]);
    }
}
