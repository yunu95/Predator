#include "InWanderLand.h"

void UIElement::Start()
{
    application::contents::ContentsLayer* contentsLayer = dynamic_cast<application::contents::ContentsLayer*>(application::Application::GetInstance().GetContentsLayer());
    // 만약 숫자 UI라면 먼저 숫자들을 모두 비활성화
    //if (!numberSetBefore)
    //{
    if (!numberSetBefore && importedUIData.customFlags & (int)UIExportFlag::IsNumber)
        SetNumber(0);
    /*for (auto each : digits)
    {
        each->DisableElementInstant();
    }*/
    //}
}
void UIElement::Update()
{
    if (worldParent)
    {
        worldParent->GetTransform()->SetWorldPosition(UIManager::Instance().GetUIPosFromWorld(worldParentOrigin));
    }
};
void UIElement::EnableElement()
{
    if (importedUIData.enumID == (int)UIEnumID::BlackMask_Alpha)
    {
        int a = 3;
    }
    if (enabled && !(importedUIData.customFlags2 & (int)UIExportFlag2::RedundantEnable))
    {
        return;
    }
    if (importedUIData.enumID == (int)UIEnumID::StatusBar_Boss_Tactic)
    {
        auto parent = GetGameObject()->GetParentGameObject();
        auto parent2 = GetGameObject()->GetParentGameObject();
    }
    enabled = true;
    GetGameObject()->SetSelfActive(true);
    if (colorTintOnDisable)
    {
        imageComponent.lock()->GetGI().SetColor(colorTintOnDisable->startColor);
        colorTintOnDisable->StopTimer();
    }
    if (scalePopDownTransition)
        scalePopDownTransition->StopTimer();
    if (soundOnDisable)
        soundOnDisable->StopTimer();
    if (disableTransition)
        disableTransition->StopTimer();
    if (linearClippingTimerOnDisable)
    {
        imageComponent.lock()->GetGI().SetLinearClippingStartPoint(0, 0);
        imageComponent.lock()->GetGI().SetLinearClippingDirection(0, -1);
        linearClippingTimerOnDisable->StopTimer();
    }

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
        musicPlayTimer->fadeInTime = importedUIData.floats[JsonUIFloatType::musicPlayOnEnable_fadeIn];
        musicPlayTimer->fadeOutTime = importedUIData.floats[JsonUIFloatType::musicPlayOnEnable_fadeOut];
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
        float targetVolume = SoundSystem::GetMusicVolume() * importedUIData.floats[JsonUIFloatType::musicMultiplyVolumeOnEnableDisable_enableFactor];
        float fadeDuration = importedUIData.floats[JsonUIFloatType::musicMultiplyVolumeOnEnableDisable_enableFactor];
        ContentsCoroutine::FadeMusicVolume(targetVolume, fadeDuration);
    }
    if (auto rot = rotator.lock())
    {
        rot->zRot = rot->initialRotation;
        rot->Update();
    }
    if (auto vid = uiVideoPlayer.lock())
    {
        vid->Enable();
    }
    for (auto each : children)
    {
        if (each->enabled)
        {
            each->enabled = false;
            each->EnableElement();
        }
    }
    for (auto each : enablePropagationTargets)
    {
        //each->enabled = false;
        if (!each->reallyDisabled)
        {
            each->EnableElement();
        }
    }
    for (auto each : exclusiveEnableGroup)
    {
        if (each != this)
        {
            each->DisableElement();
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
        musicPlayTimer->fadeInTime = importedUIData.floats[JsonUIFloatType::musicPlayOnDisable_fadeIn];
        musicPlayTimer->fadeOutTime = importedUIData.floats[JsonUIFloatType::musicPlayOnDisable_fadeOut];
        musicPlayTimer->musicPath = importedUIData.musicPlayOnDisable_musicClip;
        musicPlayTimer->Init();
        musicPlayTimer->ActivateTimer();
    }
    if (disablingHandled == false)
    {
        GetGameObject()->SetSelfActive(false);
    }
    if (parentPriorityLayout)
    {
        parentPriorityLayout->DisableChildUI(GetGameObject());
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
        if (importedUIData.floats[JsonUIFloatType::musicMultiplyVolumeOnEnableDisable_enableFactor] == 0)
        {
            ContentsCoroutine::FadeMusicVolume(1, importedUIData.floats[JsonUIFloatType::musicMultiplyVolumeOnEnableDisable_enableFactor]);
        }
        else
        {
            float targetVolume = SoundSystem::GetMusicVolume() * 1 / importedUIData.floats[JsonUIFloatType::musicMultiplyVolumeOnEnableDisable_enableFactor];
            ContentsCoroutine::FadeMusicVolume(targetVolume, importedUIData.floats[JsonUIFloatType::musicMultiplyVolumeOnEnableDisable_enableFactor]);
        }
    }
    for (auto each : disablePropagationTargets)
    {
        each->DisableElement();
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
                digits[i]->DisableElementInstant();
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
        for (auto each : digits)
        {
            each->DisableElement();
        }
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
void UIElement::SetAsWorldSpaceUI(const Vector3d& origin)
{
    if (!worldParent)
    {
        worldParent = Scene::getCurrentScene()->AddGameObject()->GetTransform();
    }
    worldParentOrigin = origin;
    worldParent->SetWorldPosition(UIManager::Instance().GetUIPosFromWorld(worldParentOrigin));
    GetGameObject()->SetParent(worldParent->GetGameObject());
}
void UIElement::SetAsScreenSpaceUI()
{
    if (worldParent)
    {
        Scene::getCurrentScene()->DestroyGameObject(worldParent->GetGameObject());
        worldParent = nullptr;
    }
}
void UIElement::DisableElementInstant()
{
    GetGameObject()->SetSelfActive(false);
    enabled = false;
    if (parentPriorityLayout)
    {
        parentPriorityLayout->DisableChildUI(GetGameObject());
    }
}
