#include "InWanderLand.h"

void UIElement::Start()
{
    application::contents::ContentsLayer* contentsLayer = dynamic_cast<application::contents::ContentsLayer*>(application::Application::GetInstance().GetContentsLayer());
    // 게임이 끝나면 삭제되도록 설정
    /*if (GetGameObject()->GetParentGameObject() == nullptr)
    {
        contentsLayer->RegisterToEditorObjectContainer(GetGameObject());
    }*/
    // 만약 숫자 UI라면 먼저 숫자들을 모두 비활성화
    if (!numberSetBefore)
    {
        for (auto each : digits)
        {
            each->DisableElementInstant();
        }
    }
};
void UIElement::EnableElement()
{
    if (enabled && !(importedUIData.customFlags2 & (int)UIExportFlag2::RedundantEnable))
    {
        return;
    }
    enabled = true;
    GetGameObject()->SetSelfActive(true);
    if (colorTintOnDisable)
        colorTintOnDisable->StopTimer();
    if (scalePopDownTransition)
        scalePopDownTransition->StopTimer();
    if (soundOnDisable)
        soundOnDisable->StopTimer();
    if (disableTransition)
        disableTransition->StopTimer();
    if (linearClippingTimerOnDisable)
        linearClippingTimerOnDisable->StopTimer();

    if (disableAfterEnable)
    {
        disableAfterEnable->ActivateTimer();
    }
    if (colorTintOnEnable)
    {
        colorTintOnEnable->ActivateTimer();
    }
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
    if (spriteAnimationOnEnable != nullptr)
    {
        spriteAnimationOnEnable->ActivateTimer();
    }
    if (importedUIData.customFlags2 & (int)UIExportFlag2::PlayMusicOnEnable)
    {
        auto musicPlayTimer = Scene::getCurrentScene()->AddGameObject()->AddComponent<PlayMusicTimer>();
        musicPlayTimer->fadeInTime = importedUIData.musicPlayOnEnable_fadeIn;
        musicPlayTimer->fadeOutTime = importedUIData.musicPlayOnEnable_fadeOut;
        musicPlayTimer->musicPath = importedUIData.musicPlayOnEnable_musicClip;
        musicPlayTimer->Init();
        musicPlayTimer->ActivateTimer();
    }
    if (linearClippingTimerOnEnable != nullptr)
    {
        linearClippingTimerOnEnable->ActivateTimer();
    }
    if (parentPriorityLayout)
    {
        parentPriorityLayout->EnableChildUI(GetGameObject());
    }
    if (timePauseOnEnable)
    {
        timePauseOnEnable->ActivateTimer();
    }
    if (importedUIData.customFlags2 & (int)UIExportFlag2::PauseMusicOnEnable)
    {
        SoundSystem::PauseMusic();
    }
    if (importedUIData.customFlags2 & (int)UIExportFlag2::MultiplyMusicVolumeOnEnableDisable)
    {
        SoundSystem::SetMusicVolume(SoundSystem::GetMusicVolume() * importedUIData.musicMultiplyVolumeOnEnableDisable_enableFactor);
    }
    for (auto each : children)
    {
        if (each->enabled)
        {
            each->enabled = false;
            each->EnableElement();
        }
    }
}
void UIElement::DisableElement()
{
    bool disablingHandled = false;
    if (!enabled)
    {
        return;
    }
    enabled = false;

    if (colorTintOnEnable)
        colorTintOnEnable->StopTimer();
    if (scalePopUpTransition)
        scalePopUpTransition->StopTimer();
    if (enableTransition)
        enableTransition->StopTimer();
    if (soundOnEnable)
        soundOnEnable->StopTimer();
    if (linearClippingTimerOnEnable)
        linearClippingTimerOnEnable->StopTimer();
    if (disableAfterEnable)
        disableAfterEnable->StopTimer();

    if (colorTintOnDisable)
    {
        disablingHandled = true;
        colorTintOnDisable->ActivateTimer();
    }
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
    if (linearClippingTimerOnDisable != nullptr)
    {
        disablingHandled = true;
        linearClippingTimerOnDisable->ActivateTimer();
    }
    if (soundOnDisable != nullptr)
    {
        soundOnDisable->ActivateTimer();
    }
    if (importedUIData.customFlags2 & (int)UIExportFlag2::PlayMusicOnDisable)
    {
        auto musicPlayTimer = Scene::getCurrentScene()->AddGameObject()->AddComponent<PlayMusicTimer>();
        musicPlayTimer->fadeInTime = importedUIData.musicPlayOnDisable_fadeIn;
        musicPlayTimer->fadeOutTime = importedUIData.musicPlayOnDisable_fadeOut;
        musicPlayTimer->musicPath = importedUIData.musicPlayOnDisable_musicClip;
        musicPlayTimer->Init();
        musicPlayTimer->ActivateTimer();
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
    if (importedUIData.customFlags2 & (int)UIExportFlag2::UnPauseMusicOnDisable)
    {
        SoundSystem::UnpauseMusic();
    }
    if (importedUIData.customFlags2 & (int)UIExportFlag2::MultiplyMusicVolumeOnEnableDisable)
    {
        if (importedUIData.musicMultiplyVolumeOnEnableDisable_enableFactor == 0)
        {
            SoundSystem::SetMusicVolume(1);
        }
        else
        {
            SoundSystem::SetMusicVolume(SoundSystem::GetMusicVolume() * 1 / importedUIData.musicMultiplyVolumeOnEnableDisable_enableFactor);
        }
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
                digits[i]->imageComponent.lock()->GetGI().SetImage((*digitFont)[numberAsInt % 10]);
                numberAsInt /= 10;
            }
        }
    }
    else
    {
        if (importedUIData.numberShowZero)
        {
            digits[0]->EnableElement();
            digits[0]->imageComponent.lock()->GetGI().SetImage((*digitFont)[0]);
        }
        else
        {
            digits[0]->DisableElement();
            digits[0]->imageComponent.lock()->GetGI().SetImage((*digitFont)[0]);
        }
    }
}
void UIElement::DisableElementInstant()
{
    GetGameObject()->SetSelfActive(false);
    enabled = false;
}

//void UIElement::PlayFunction()
//{
//
//}
//
//void UIElement::StopFunction()
//{
//    yunutyEngine::Scene::getCurrentScene()->DestroyGameObject(GetGameObject());
//}
