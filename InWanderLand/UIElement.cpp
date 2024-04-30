#include "InWanderLand.h"

void UIElement::Start()
{
    application::contents::ContentsLayer* contentsLayer = dynamic_cast<application::contents::ContentsLayer*>(application::Application::GetInstance().GetContentsLayer());
    // 게임이 끝나면 삭제되도록 설정
    if (GetGameObject()->GetParentGameObject() == nullptr)
    {
        contentsLayer->RegisterToEditorObjectContainer(GetGameObject());
    }
    // 만약 숫자 UI라면 먼저 숫자들을 모두 비활성화
    if (!numberSetBefore)
    {
        for (auto each : digits)
        {
            each->GetGameObject()->SetSelfActive(false);
        }
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
    if (timePauseOnEnable)
    {
        timePauseOnEnable->ActivateTimer();
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
    if (importedUIData.customFlags & (int)UIExportFlag::TimeContinueOnDisable)
    {
        if (timePauseOnEnable)
        {
            timePauseOnEnable->StopTimer();
        }
        Time::SetTimeScale(1);
    }
    if (parentPriorityLayout)
    {
        parentPriorityLayout->DisableChildUI(GetGameObject());
    }
}
void UIElement::SetNumber(float number)
{
    numberSetBefore = true;
    int numberAsInt{ static_cast<int>(number) };
    if (importedUIData.numberCeil)
        numberAsInt = ceilf(number);
    if (numberAsInt < 0)
        numberAsInt = 0;

    assert(digitFont);
    int maxDigit = log10(numberAsInt);
    assert(digits.size() > log10(numberAsInt));
    if (numberAsInt > 0)
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
                digits[i]->imageComponent->GetGI().SetImage((*digitFont)[numberAsInt % 10]);
                numberAsInt /= 10;
            }
        }
    }
    else
    {
        if (importedUIData.numberShowZero)
        {
            digits[0]->EnableElement();
            digits[0]->imageComponent->GetGI().SetImage((*digitFont)[0]);
        }
        else
        {
            digits[0]->DisableElement();
            digits[0]->imageComponent->GetGI().SetImage((*digitFont)[0]);
        }
    }
}
