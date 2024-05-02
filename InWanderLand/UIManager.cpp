#include "UIManager.h"
#include "RTSCam.h"
#include "UIManager.h"
#include "UIImage.h"
#include "InWanderLand.h"
#include "PopupOnEnable.h"
#include "ContentsLayer.h"
#include "UIElement.h"
#include "UIOffsetTransition.h"
#include "FloatFollower.h"
#include "PulsingUI.h"
#include "UIPriorityLayout.h"
#include "SoundPlayingTimer.h"
#include "InWanderLand.h"
#include "SkillUpgradeSystem.h"
#include "TimePauseTImer.h"
#include <fstream>

using namespace yunutyEngine::graphics;
void UIManager::Clear()
{
    m_highestPriorityButton = nullptr;
    m_currentHighestLayer = -1;
    m_selectedButtons.clear();
    for (auto each : uisByIndex)
    {
        Scene::getCurrentScene()->DestroyGameObject(each.second->GetGameObject());
    }
}
void UIManager::FadeOutRight(float duration)
{
    if (auto elm = UIManager::Instance().GetUIElementByEnum(UIEnumID::BlackMask_LeftToRight); !elm->GetGameObject()->GetActive())
    {
        elm->enableTransition->m_duration = duration;
        elm->EnableElement();
    }
}
void UIManager::FadeOutLeft(float duration)
{
    if (auto elm = UIManager::Instance().GetUIElementByEnum(UIEnumID::BlackMask_RightToLeft); !elm->GetGameObject()->GetActive())
    {
        elm->enableTransition->m_duration = duration;
        elm->EnableElement();
    }
}
void UIManager::FadeOutBottom(float duration)
{
    if (auto elm = UIManager::Instance().GetUIElementByEnum(UIEnumID::BlackMask_TopToBottom); !elm->GetGameObject()->GetActive())
    {
        elm->enableTransition->m_duration = duration;
        elm->EnableElement();
    }
}
void UIManager::FadeOutTop(float duration)
{
    if (auto elm = UIManager::Instance().GetUIElementByEnum(UIEnumID::BlackMask_BottomToTop); !elm->GetGameObject()->GetActive())
    {
        elm->enableTransition->m_duration = duration;
        elm->EnableElement();
    }
}
void UIManager::FadeIn(float duration)
{
    if (auto elm = UIManager::Instance().GetUIElementByEnum(UIEnumID::BlackMask_TopToBottom); elm->GetGameObject()->GetActive())
    {
        elm->disableTransition->m_duration = duration;
        elm->DisableElement();
    }
    if (auto elm = UIManager::Instance().GetUIElementByEnum(UIEnumID::BlackMask_RightToLeft); elm->GetGameObject()->GetActive())
    {
        elm->disableTransition->m_duration = duration;
        elm->DisableElement();
    }
    if (auto elm = UIManager::Instance().GetUIElementByEnum(UIEnumID::BlackMask_BottomToTop); elm->GetGameObject()->GetActive())
    {
        elm->disableTransition->m_duration = duration;
        elm->DisableElement();
    }
    if (auto elm = UIManager::Instance().GetUIElementByEnum(UIEnumID::BlackMask_LeftToRight); elm->GetGameObject()->GetActive())
    {
        elm->disableTransition->m_duration = duration;
        elm->DisableElement();
    }
}
void UIManager::SetIngameUIVisible(bool visible)
{
    if (visible)
    {
        UIManager::Instance().GetUIElementByEnum(UIEnumID::Ingame_Bottom_Layout)->EnableElement();
        UIManager::Instance().GetUIElementByEnum(UIEnumID::Ingame_MenuButton)->EnableElement();
        UIManager::Instance().GetUIElementByEnum(UIEnumID::LetterBox_Bottom)->DisableElement();
        UIManager::Instance().GetUIElementByEnum(UIEnumID::LetterBox_Top)->DisableElement();
    }
    else
    {
        UIManager::Instance().GetUIElementByEnum(UIEnumID::Ingame_Bottom_Layout)->DisableElement();
        UIManager::Instance().GetUIElementByEnum(UIEnumID::Ingame_MenuButton)->DisableElement();
        UIManager::Instance().GetUIElementByEnum(UIEnumID::LetterBox_Bottom)->EnableElement();
        UIManager::Instance().GetUIElementByEnum(UIEnumID::LetterBox_Top)->EnableElement();
    }
}

void UIManager::ReportButtonOnMouse(UIButton* p_btn)
{
    m_selectedButtons.insert(p_btn);
    UpdateHighestPriorityButton();
}

void UIManager::ReportMouseExitButton(UIButton* p_btn)
{
    m_selectedButtons.erase(p_btn);
    UpdateHighestPriorityButton();
}
void UIManager::ShowComboObjectives()
{
    uisByEnumID[UIEnumID::Ingame_Combo_DescriptionTitleImg]->EnableElement();
    uisByEnumID[UIEnumID::Ingame_Combo_Description1]->EnableElement();
    uisByEnumID[UIEnumID::Ingame_Combo_Description2]->EnableElement();
    uisByEnumID[UIEnumID::Ingame_Combo_Description3]->EnableElement();
}
void UIManager::HideComboObjectvies()
{
    uisByEnumID[UIEnumID::Ingame_Combo_DescriptionTitleImg]->DisableElement();
    uisByEnumID[UIEnumID::Ingame_Combo_Description1]->DisableElement();
    uisByEnumID[UIEnumID::Ingame_Combo_Description2]->DisableElement();
    uisByEnumID[UIEnumID::Ingame_Combo_Description3]->DisableElement();
}
void UIManager::UpdateHighestPriorityButton()
{
    // 현재 하이라이트된 버튼이 적법한 버튼이라면, 더 이상의 처리는 필요없습니다.
    if (!m_selectedButtons.empty() && m_highestPriorityButton == *m_selectedButtons.begin())
        return;
    // 하이라이트된 버튼이 바뀌어야 한다면, 기존 버튼의 Exit 이벤트부터 처리합니다.
    if (m_highestPriorityButton)
    {
        //m_highestPriorityButton->m_ImageComponent->GetGI().SetImage(m_highestPriorityButton->m_IdleImage);
        if (m_highestPriorityButton->m_onMouseExitFunction)
        {
            m_highestPriorityButton->m_onMouseExitFunction();
        }
    }
    // 혹시 앞 버튼의 Exit 이벤트로 인해 새로 하이라이트된 버튼이 비활성화될수도 있으니, 비활성화된 버튼은 제거합니다. 
    while (!m_selectedButtons.empty() && (*m_selectedButtons.begin())->GetGameObject()->GetActive() == false)
    {
        m_selectedButtons.erase(*m_selectedButtons.begin());
    }
    m_highestPriorityButton = m_selectedButtons.empty() ? nullptr : *m_selectedButtons.begin();

    if (isButtonActiviated = m_highestPriorityButton != nullptr)
    {
        if (m_highestPriorityButton->m_onMouseFunction)
            m_highestPriorityButton->m_onMouseFunction();
    }
}

bool UIManager::IsMouseOnButton()
{
    return isButtonActiviated;
}
weak_ptr<UIElement> UIManager::DuplicateUIElement(UIElement* ui)
{
    auto retVal = Scene::getCurrentScene()->AddGameObject()->AddComponentAsWeakPtr<UIElement>();
    localContext = retVal.lock().get();
    ImportDefaultAction(ui->importedUIData, retVal.lock().get());
    for (auto& eachData : ui->localUIdatasByIndex)
    {
        auto uiObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
        auto uiElement = uiObject->AddComponent<UIElement>();
        ImportDefaultAction(eachData.second, uiElement);
    }
    ImportDealWithSpecialCases(retVal.lock().get()->importedUIData, retVal.lock().get());
    for (auto& eachUI : retVal.lock().get()->localUIsByIndex)
    {
        ImportDealWithSpecialCases(eachUI.second->importedUIData, retVal.lock().get());
    }
    ImportDefaultAction_Post(ui->importedUIData, retVal.lock().get());
    for (auto& eachUI : retVal.lock().get()->localUIsByIndex)
    {
        ImportDefaultAction_Post(eachUI.second->importedUIData, eachUI.second);
    }
    ImportDealWithSpecialCases_Post(retVal.lock().get()->importedUIData, retVal.lock().get());
    for (auto& eachUI : retVal.lock().get()->localUIsByIndex)
    {
        ImportDealWithSpecialCases_Post(eachUI.second->importedUIData, eachUI.second);
    }
    localContext = nullptr;

    return retVal;
}
UIElement* UIManager::GetUIElementWithIndex(int index)
{
    if (localContext)
    {
        if (auto itr = localContext->localUIsByIndex.find(index); itr != localContext->localUIsByIndex.end())
            return itr->second;
    }
    else
    {
        if (auto itr = uisByIndex.find(index); itr != uisByIndex.end())
            return itr->second;
    }
}
JsonUIData UIManager::GetUIDataWithIndex(int index)
{
    if (localContext)
    {
        if (auto itr = localContext->localUIdatasByIndex.find(index); itr != localContext->localUIdatasByIndex.end())
            return itr->second;
    }
    else
    {
        if (auto itr = uidatasByIndex.find(index); itr != uidatasByIndex.end())
            return itr->second;
    }
}
UIElement* UIManager::GetUIElementByEnum(UIEnumID uiEnumID)
{
    if (localContext)
    {
        if (auto itr = localContext->localUIsByEnumID.find(uiEnumID); itr != localContext->localUIsByEnumID.end())
            return itr->second;
    }
    else
    {
        if (auto itr = uisByEnumID.find(uiEnumID); itr != uisByEnumID.end())
            return itr->second;
    }
    assert(false);
    return nullptr;
}
void UIManager::SetUIElementWithEnum(UIEnumID uiEnumID, UIElement* ui)
{
    if (localContext)
    {
        localContext->localUIsByEnumID[uiEnumID] = ui;
    }
    else
    {
        uisByEnumID[uiEnumID] = ui;
    }
}
void UIManager::SetUIElementWithIndex(int index, UIElement* ui)
{
    if (localContext)
    {
        localContext->localUIsByIndex[index] = ui;
    }
    else
    {
        uisByIndex[index] = ui;
    }
}
void UIManager::SetUIDataWithIndex(int index, const JsonUIData& uiData)
{
    if (localContext)
    {
        localContext->localUIdatasByIndex[index] = uiData;
    }
    else
    {
        uidatasByIndex[index] = uiData;
    }
}
UIElement* UIManager::GetBuffIcon(Unit* owningUnit, StatusEffect::StatusEffectEnum uiEnumID)
{
    switch (owningUnit->GetUnitType())
    {
    case Unit::UnitType::Warrior:
    {
        switch (uiEnumID)
        {
        case StatusEffect::StatusEffectEnum::Bleeding: return GetUIElementByEnum(UIEnumID::Portrait_Robin_Buff_Bleeding);
        case StatusEffect::StatusEffectEnum::Blinding: return GetUIElementByEnum(UIEnumID::Portrait_Robin_Buff_Blinding);
        case StatusEffect::StatusEffectEnum::Paralysis: return GetUIElementByEnum(UIEnumID::Portrait_Robin_Buff_Paralysis);
        case StatusEffect::StatusEffectEnum::KnockBack: return GetUIElementByEnum(UIEnumID::Portrait_Robin_Buff_KnockBack);
        case StatusEffect::StatusEffectEnum::Taunted: return GetUIElementByEnum(UIEnumID::Portrait_Robin_Buff_Taunted);
        default: assert(false);
        }
    }
    case Unit::UnitType::Magician:
    {
        switch (uiEnumID)
        {
        case StatusEffect::StatusEffectEnum::Bleeding: return GetUIElementByEnum(UIEnumID::Portrait_Ursula_Buff_Bleeding);
        case StatusEffect::StatusEffectEnum::Blinding: return GetUIElementByEnum(UIEnumID::Portrait_Ursula_Buff_Blinding);
        case StatusEffect::StatusEffectEnum::Paralysis: return GetUIElementByEnum(UIEnumID::Portrait_Ursula_Buff_Paralysis);
        case StatusEffect::StatusEffectEnum::KnockBack: return GetUIElementByEnum(UIEnumID::Portrait_Ursula_Buff_KnockBack);
        case StatusEffect::StatusEffectEnum::Taunted: return GetUIElementByEnum(UIEnumID::Portrait_Ursula_Buff_Taunted);
        default: assert(false);
        }
    }
    case Unit::UnitType::Healer:
    {
        switch (uiEnumID)
        {
        case StatusEffect::StatusEffectEnum::Bleeding: return GetUIElementByEnum(UIEnumID::Portrait_Hansel_Buff_Bleeding);
        case StatusEffect::StatusEffectEnum::Blinding: return GetUIElementByEnum(UIEnumID::Portrait_Hansel_Buff_Blinding);
        case StatusEffect::StatusEffectEnum::Paralysis: return GetUIElementByEnum(UIEnumID::Portrait_Hansel_Buff_Paralysis);
        case StatusEffect::StatusEffectEnum::KnockBack: return GetUIElementByEnum(UIEnumID::Portrait_Hansel_Buff_KnockBack);
        case StatusEffect::StatusEffectEnum::Taunted: return GetUIElementByEnum(UIEnumID::Portrait_Hansel_Buff_Taunted);
        default: assert(false);
        }
    }
    }
    return nullptr;
}

void UIManager::Update()
{
    if (yunutyEngine::Input::isKeyPushed(yunutyEngine::KeyCode::MouseLeftClick))
    {
        if (isButtonActiviated)
        {
            if (m_highestPriorityButton)
            {
                m_highestPriorityButton->m_mousePushedFunction();
            }
        }
    }

    if (yunutyEngine::Input::isKeyLifted(yunutyEngine::KeyCode::MouseLeftClick))
    {
        if (m_highestPriorityButton != nullptr)
        {
            m_highestPriorityButton->m_mouseLiftedFunction();
        }
    }
}

void UIManager::Start()
{
    isSingletonComponent = true;
}

void UIManager::PlayFunction()
{
    this->SetActive(true);
    if (isOncePaused)
    {
        Start();
    }
}

void UIManager::StopFunction()
{
    Clear();
}

void UIManager::ImportUI(const char* path)
{
    std::ifstream file{ path };
    if (file.is_open())
    {
        json data;
        file >> data;
        assert(data.contains("dataList"));
        UUID uuid;
        uiImportingPriority = 0;
        for (auto& each : data["dataList"].items())
        {
            auto key = each.key();
            JsonUIData uiData;
            application::FieldPreDecoding<boost::pfr::tuple_size_v<JsonUIData>>(uiData, each.value());

            auto uiObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
            auto uiElement = uiObject->AddComponent<UIElement>();
            uiElement->importedUIData = uiData;

            if (ImportDealWithSpecialCases(uiData, uiElement) == false)
            {
                ImportDefaultAction(uiData, uiElement);
            }
            uiImportingPriority++;
        }
        uiImportingPriority = 0;
        for (auto& each : data["dataList"].items())
        {
            auto key = each.key();
            JsonUIData uiData;
            application::FieldPreDecoding<boost::pfr::tuple_size_v<JsonUIData>>(uiData, each.value());
            if (ImportDealWithSpecialCases_Post(uiData, uisByIndex[uiData.uiIndex]) == false)
            {
                ImportDefaultAction_Post(uiData, uisByIndex[uiData.uiIndex]);
            }
            uiImportingPriority++;
        }
    }
}

// JsonUIData만으로 UI를 생성합니다.
void UIManager::ImportDefaultAction(const JsonUIData& uiData, UIElement* element)
{
    auto uiObject = element->GetGameObject();
    auto rsrcMgr = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
    UIImage* uiImageComponent{ nullptr };
    UIButton* uiButtonComponent{ nullptr };
    yunuGI::ITexture* idleTexture{ nullptr };
    //uiObject->GetTransform()->SetLocalScale({ 0.5,1,1 });
    //uiObject->AddComponent<PopupOnEnable>();
    if (uiData.enumID != 0)
        SetUIElementWithEnum((UIEnumID)uiData.enumID, element);
    SetUIDataWithIndex(uiData.uiIndex, uiData);
    SetUIElementWithIndex(uiData.uiIndex, element);
    if (uiData.imagePath != "")
    {
        element->imageComponent = uiObject->AddComponentAsWeakPtr<UIImage>();
        uiImageComponent = element->imageComponent.lock().get();
        idleTexture = rsrcMgr->GetTexture(yutility::GetWString(uiData.imagePath).c_str());
        if (idleTexture == nullptr)
        {
            idleTexture = rsrcMgr->GetTexture(L"Texture/zoro.jpg");
        }
        uiImageComponent->GetGI().SetImage(idleTexture);
        uiImageComponent->GetGI().SetWidth(uiData.width);
        uiImageComponent->GetGI().SetHeight(uiData.height);
        // apply pivot
        uiImageComponent->GetGI().SetXPivot(uiData.pivot[0]);
        uiImageComponent->GetGI().SetYPivot(1 - uiData.pivot[1]);
        uiImageComponent->GetGI().SetLayer(uiImportingPriority);
        uiImageComponent->GetGI().SetColor(yunuGI::Color{ uiData.color[0],uiData.color[1],uiData.color[2],uiData.color[3] });

        if (!(uiData.customFlags & (int)UIExportFlag::NoOverlaying))
        {
            uiButtonComponent = element->button = uiObject->AddComponent<UIButton>();
            uiButtonComponent->SetImageComponent(uiImageComponent);
            //uiButtonComponent->SetIdleImage(idleTexture);
            //uiButtonComponent->SetOnMouseImage(rsrcMgr->GetTexture(L"Texture/zoro.jpg"));
        }
        else
        {
            //int a = 3;
        }
    }
    if (uiData.customFlags & (int)UIExportFlag::CanAdjustHeight)
    {
        assert(!element->adjuster);
        element->adjuster = uiObject->AddComponent<FloatFollower>();
        element->adjuster->SetFollowingRate(uiData.adjustingRate);
        element->adjuster->applier = [=](float val)
            {
                uiImageComponent->GetGI().SetHeight(val * uiData.height);
            };
    }
    if (uiData.customFlags & (int)UIExportFlag::CanAdjustWidth)
    {
        assert(!element->adjuster);
        element->adjuster = uiObject->AddComponent<FloatFollower>();
        element->adjuster->SetFollowingRate(uiData.adjustingRate);
        element->adjuster->applier = [=](float val)
            {
                uiImageComponent->GetGI().SetWidth(val * uiData.width);
            };
    }
    if (uiData.customFlags & (int)UIExportFlag::CanAdjustRadialFill)
    {
        assert(!element->adjuster);
        // 위를 덮어씌우는 이미지
        element->imageComponent.lock()->GetGI().SetRadialFillMode(true);
        element->imageComponent.lock()->GetGI().SetRadialFillDegree(0);
        element->imageComponent.lock()->GetGI().SetRadialFillDirection(false);
        element->imageComponent.lock()->GetGI().SetRadialFillStartPoint(0, 1);
        element->adjuster = uiObject->AddComponent<FloatFollower>();
        element->adjuster->SetFollowingRate(uiData.adjustingRate);
        element->adjuster->applier = [=](float val)
            {
                element->imageComponent.lock()->GetGI().SetRadialFillDegree(val * 360);
            };
    }
    if (uiData.customFlags & (int)UIExportFlag::IsDigitFont)
    {
        digitFonts[element] = std::array<yunuGI::ITexture*, 10>{};
    }
    if (uiData.customFlags & (int)UIExportFlag::TimeStopOnEnable)
    {
        element->timePauseOnEnable = uiObject->AddComponent<TimePauseTimer>();
        element->timePauseOnEnable->m_duration = uiData.timeStoppingDuration;
        element->timePauseOnEnable->Init();
    };
    if (uiData.customFlags & (int)UIExportFlag::ColorTintOnEnable)
    {
        element->colorTintOnEnable = uiObject->AddComponent<ColorTintTimer>();
        element->colorTintOnEnable->uiImage = element->imageComponent.lock().get();
        element->colorTintOnEnable->startColor = yunuGI::Color{ uiData.colorTintOnEnableStart[0],uiData.colorTintOnEnableStart[1],uiData.colorTintOnEnableStart[2],uiData.colorTintOnEnableStart[3] };
        element->colorTintOnEnable->endColor = yunuGI::Color{ uiData.colorTintOnEnableEnd[0],uiData.colorTintOnEnableEnd[1],uiData.colorTintOnEnableEnd[2],uiData.colorTintOnEnableEnd[3] };
        element->colorTintOnEnable->m_duration = uiData.colorTintOnEnableDuration;
        element->colorTintOnEnable->uiCurveType = uiData.colorTintOnEnableCurveType;
        element->colorTintOnEnable->disableOnEnd = false;
        element->colorTintOnEnable->Init();
    };
    if (uiData.customFlags & (int)UIExportFlag::ColorTintOnDisable)
    {
        element->colorTintOnDisable = uiObject->AddComponent<ColorTintTimer>();
        element->colorTintOnDisable->uiImage = element->imageComponent.lock().get();
        element->colorTintOnDisable->startColor = yunuGI::Color{ uiData.colorTintOnDisableStart[0],uiData.colorTintOnDisableStart[1],uiData.colorTintOnDisableStart[2],uiData.colorTintOnDisableStart[3] };
        element->colorTintOnDisable->endColor = yunuGI::Color{ uiData.colorTintOnDisableStart[0],uiData.colorTintOnDisableStart[1],uiData.colorTintOnDisableStart[2],uiData.colorTintOnDisableStart[3] };
        element->colorTintOnDisable->m_duration = uiData.colorTintOnDisableDuration;
        element->colorTintOnDisable->uiCurveType = uiData.colorTintOnDisableCurveType;
        element->colorTintOnDisable->disableOnEnd = false;
        element->colorTintOnEnable->Init();
    };
    if (uiData.customFlags2 & (int)UIExportFlag2::LinearClipOnEnable)
    {
        element->linearClippingTimerOnEnable = uiObject->AddComponent<LinearClippingTimer>();
        element->linearClippingTimerOnEnable->uiImage = element->imageComponent.lock().get();
        element->linearClippingTimerOnEnable->clipDirection = { uiData.linearClipOnEnableDir[0], uiData.linearClipOnEnableDir[1] };
        element->linearClippingTimerOnEnable->disableOnEnd = false;
        element->linearClippingTimerOnEnable->m_duration = uiData.linearClipOnEnableDuration;
        element->linearClippingTimerOnEnable->startPos = { uiData.linearClipOnEnableStart[0], uiData.linearClipOnEnableStart[1] };
        element->linearClippingTimerOnEnable->uiCurveType = uiData.linearClipOnEnableCurveType;
        element->linearClippingTimerOnEnable->reverseOffset = false;
        element->linearClippingTimerOnEnable->Init();
    };
    if (uiData.customFlags2 & (int)UIExportFlag2::LinearClipOnDisable)
    {
        element->linearClippingTimerOnDisable = uiObject->AddComponent<LinearClippingTimer>();
        element->linearClippingTimerOnDisable->uiImage = element->imageComponent.lock().get();
        element->linearClippingTimerOnDisable->clipDirection = { uiData.linearClipOnDisableDir[0], uiData.linearClipOnDisableDir[1] };
        element->linearClippingTimerOnDisable->disableOnEnd = true;
        element->linearClippingTimerOnDisable->m_duration = uiData.linearClipOnDisableDuration;
        element->linearClippingTimerOnDisable->startPos = { uiData.linearClipOnDisableStart[0], uiData.linearClipOnDisableStart[1] };
        element->linearClippingTimerOnDisable->uiCurveType = uiData.linearClipOnDisableCurveType;
        element->linearClippingTimerOnDisable->reverseOffset = true;
        element->linearClippingTimerOnDisable->Init();
    };

    Vector3d pivotPos{ 0,0,0 };
    // offset by anchor
    Vector3d parentSize{ 1920, 1080, 0 };
    Vector2d parentPivot{ 0.0,0.0 };
    if (uiData.parentUIIndex != -1)
    {
        auto parentData = GetUIDataWithIndex(uiData.parentUIIndex);
        auto parent = GetUIElementWithIndex(uiData.parentUIIndex)->GetGameObject();
        parentSize.x = parentData.width;
        parentSize.y = parentData.height;
        parentPivot.x = parentData.pivot[0];
        parentPivot.y = 1 - parentData.pivot[1];
        uiObject->SetParent(parent);
    };
    // offset by offset
    pivotPos.x += uiData.anchoredPosition[0];
    pivotPos.y -= uiData.anchoredPosition[1];

    pivotPos.x += parentSize.x * (uiData.anchor[0] - parentPivot.x);
    pivotPos.y += parentSize.y * (1 - uiData.anchor[1] - parentPivot.y);

    //topLeftPos.x -= uiData.pivot[0] * uiData.width;
    //topLeftPos.y -= (1 - uiData.pivot[1]) * uiData.height;
    uiObject->GetTransform()->SetLocalPosition({ pivotPos.x, pivotPos.y, 0 });
}
void UIManager::ImportDefaultAction_Post(const JsonUIData& uiData, UIElement* element)
{
    UIButton* button{ element->button };
    transform(element->GetGameObject()->GetChildren().begin(), element->GetGameObject()->GetChildren().end(), back_inserter(element->children), [](auto each) {return each->GetComponent<UIElement>(); });
    // 만약 닫기 버튼이라면...
    if (uiData.customFlags & (int)UIExportFlag::CloseButton)
    {
        vector<UIElement*> closeTargets;
        std::transform(uiData.closeTargets.begin(), uiData.closeTargets.end(), std::back_inserter(closeTargets), [=](int id) {return GetUIElementWithIndex(id); });
        button->AddButtonClickFunction([=]()
            {
                for (auto each : closeTargets)
                {
                    each->DisableElement();
                }
            });
    }
    // 만약 열기 버튼이라면...
    if (uiData.customFlags & (int)UIExportFlag::OpeningButton)
    {
        vector<UIElement*> openTargets;
        std::transform(uiData.openTargets.begin(), uiData.openTargets.end(), std::back_inserter(openTargets), [=](int id) {return GetUIElementWithIndex(id); });
        if (uiData.customFlags & (int)UIExportFlag::IsToggle)
        {
            button->AddButtonClickFunction([targets = std::move(openTargets)]()
                {
                    for (auto openTarget : targets)
                    {
                        if (!openTarget->GetGameObject()->GetSelfActive())
                        {
                            openTarget->EnableElement();
                        }
                        else
                        {
                            openTarget->DisableElement();
                        }
                    }
                });
        }
        else
        {
            button->AddButtonClickFunction([targets = std::move(openTargets)]()
                {
                    for (auto openTarget : targets)
                    {
                        openTarget->EnableElement();
                    }
                });
        }
    }
    // 만약 Disabling button이라면...
    if (uiData.customFlags & (int)UIExportFlag::CloseButton)
    {
        vector<UIElement*> closeTargets;
        std::transform(uiData.closeTargets.begin(), uiData.closeTargets.end(), std::back_inserter(closeTargets), [=](int id) {return GetUIElementWithIndex(id); });
        button->AddButtonClickFunction([targets = std::move(closeTargets)]()
            {
                for (auto closeTarget : targets)
                {
                    assert(closeTarget);
                    closeTarget->DisableElement();
                }
            });
    }
    static constexpr int priority_Tooltip = 123456789;
    // 만약 툴팁을 포함하는 UI라면...
    if (uiData.customFlags & (int)UIExportFlag::IsIncludingTooltips)
    {
        vector<UIElement*> tooltipTargets;
        std::transform(uiData.hoverEnableTargets.begin(), uiData.hoverEnableTargets.end(), std::back_inserter(tooltipTargets), [&](int idx) {return GetUIElementWithIndex(idx); });
        for (auto each : tooltipTargets)
        {
            each->GetGameObject()->SetSelfActive(false);
            if (auto img = each->imageComponent; img.lock())
            {
                img.lock()->GetGI().SetLayer(priority_Tooltip);
            }
        }
        button->AddButtonOnMouseFunction([=]()
            {
                for (auto each : tooltipTargets)
                {
                    each->EnableElement();
                }
            });
        button->m_onMouseExitFunction = [=]()
            {
                for (auto each : tooltipTargets)
                {
                    each->DisableElement();
                }
            };
    }
    // 만약 등장 애니메이션이 들어갔다면...
    if (uiData.customFlags & (int)UIExportFlag::IsPoppingUp)
    {
        element->scalePopUpTransition = element->GetGameObject()->AddComponent<PopupOnEnable>();
        element->scalePopUpTransition->m_duration = uiData.popUpDuration;
        element->scalePopUpTransition->x = uiData.popUpX;
        element->scalePopUpTransition->y = uiData.popUpY;
        element->scalePopUpTransition->z = uiData.popUpZ;
        element->scalePopUpTransition->Init();
    }
    if (uiData.customFlags & (int)UIExportFlag::IsPoppingDown)
    {
        element->scalePopDownTransition = element->GetGameObject()->AddComponent<PopDownOnDisable>();
        element->scalePopDownTransition->m_duration = uiData.popDownDuration;
        element->scalePopDownTransition->x = uiData.popDownX;
        element->scalePopDownTransition->y = uiData.popDownY;
        element->scalePopDownTransition->z = uiData.popDownZ;
        element->scalePopDownTransition->Init();
    }
    if (uiData.customFlags & (int)UIExportFlag::IsPulsing)
    {
        auto pulsingUI = element->GetGameObject()->AddComponent<PulsingUI>();
        pulsingUI->m_duration = uiData.pulsingPeriod;
        pulsingUI->pulsingMin = uiData.pulsingMin;
        pulsingUI->pulsingMax = uiData.pulsingMax;
        pulsingUI->Init();
    }
    if (uiData.customFlags & (int)UIExportFlag::IsTranslatingOnEnable)
    {
        element->enableTransition = element->GetGameObject()->AddComponent<UIOffsetTransition>();
        element->enableTransition->Init(uiData, true);
    }
    if (uiData.customFlags & (int)UIExportFlag::IsTranslatingOnDisable)
    {
        element->disableTransition = element->GetGameObject()->AddComponent<UIOffsetTransition>();
        element->disableTransition->Init(uiData, false);
    }
    if (uiData.customFlags & (int)UIExportFlag::PlaySoundOnClick)
    {
        element->soundOnClick = element->GetGameObject()->AddComponent<SoundPlayingTimer>();
        element->soundOnClick->soundPath = uiData.soundOnClick;
        element->soundOnClick->m_duration = uiData.soundOnClick_delay;
        element->button->AddButtonClickFunction([=]()
            {
                element->soundOnClick->ActivateTimer();
            });
        element->soundOnClick->Init();
    }
    if (uiData.customFlags & (int)UIExportFlag::PlaySoundOnHover)
    {
        element->soundOnHover = element->GetGameObject()->AddComponent<SoundPlayingTimer>();
        element->soundOnHover->soundPath = uiData.soundOnHover;
        element->soundOnHover->m_duration = uiData.soundOnHover_delay;
        element->button->AddButtonOnMouseFunction([=]()
            {
                element->soundOnHover->ActivateTimer();
            });
        element->soundOnHover->Init();
    }
    if (uiData.customFlags & (int)UIExportFlag::PlaySoundOnEnable)
    {
        element->soundOnEnable = element->GetGameObject()->AddComponent<SoundPlayingTimer>();
        element->soundOnEnable->soundPath = uiData.soundOnEnable;
        element->soundOnEnable->m_duration = uiData.soundOnEnable_delay;
        element->soundOnEnable->Init();
    }
    if (uiData.customFlags & (int)UIExportFlag::PlaySoundOnDisable)
    {
        element->soundOnDisable = element->GetGameObject()->AddComponent<SoundPlayingTimer>();
        element->soundOnDisable->soundPath = uiData.soundOnDisable;
        element->soundOnDisable->m_duration = uiData.soundOnDisable_delay;
        element->soundOnDisable->Init();
    }
    if (uiData.customFlags & (int)UIExportFlag::PriorityLayout)
    {
        element->priorityLayout = element->GetGameObject()->AddComponent<UIPriorityLayout>();
        for (auto each : element->GetGameObject()->GetChildren())
        {
            if (auto child = each->GetComponent<UIElement>())
            {
                child->parentPriorityLayout = element->priorityLayout;
            }
            element->priorityLayout->positions.push_back(each->GetTransform()->GetLocalPosition());
        }
    }
    if (uiData.customFlags & (int)UIExportFlag::IsNumber)
    {
        element->digitFont = &digitFonts[uisByIndex[uiData.numberFontSet]];
        std::transform(uiData.numberDigits.begin(), uiData.numberDigits.end(), std::back_inserter(element->digits), [=](int id) {return GetUIElementWithIndex(id); });
    }
    if (uiData.customFlags & (int)UIExportFlag::IsDigitFont)
    {
        int number = 0;
        for (auto each : element->GetGameObject()->GetChildren())
        {
            if (auto img = each->GetComponent<UIImage>())
            {
                digitFonts[element][number++] = img->GetGI().GetImage();
            }
        }
    }
    if (uiData.customFlags & (int)UIExportFlag::IsSkillUpgrade)
    {
        UIEnumID upgradeID{ static_cast<UIEnumID>(uiData.enumID) };
        UIEnumID dependentUpgrade{ UIEnumID::None };
        if (uiData.dependentUpgrade >= 0)
        {
            dependentUpgrade = static_cast<UIEnumID>(GetUIDataWithIndex(uiData.dependentUpgrade).enumID);
        }
        element->button->AddButtonClickFunction([=]()
            {
                if (SkillUpgradeSystem::SingleInstance().IsUpgraded(static_cast<UIEnumID>(uiData.enumID)))
                {
                    return;
                }

                if (SkillUpgradeSystem::SingleInstance().GetSkillPoints() <= 0)
                {
                    GetUIElementByEnum(UIEnumID::PopUpMessage_NotEnoughSP)->EnableElement();
                }
                else
                {
                    // 선행 업그레이드까지 완료된 경우 허락창을 띄운다.
                    if (dependentUpgrade == UIEnumID::None || SkillUpgradeSystem::SingleInstance().IsUpgraded(dependentUpgrade))
                    {
                        SkillUpgradeSystem::SingleInstance().SetUpgradeTarget(upgradeID);
                        GetUIElementByEnum(UIEnumID::PopUpMessage_PermissionForUpgrade)->EnableElement();
                    }
                    else
                    {
                        GetUIElementByEnum(UIEnumID::PopUpMessage_RequirementNotMet)->EnableElement();
                    }
                }
            });
    }
    // 초기 상태가 비활성화 상태라면...
    if (uiData.customFlags & (int)UIExportFlag::DisableOnStart)
    {
        element->GetGameObject()->SetSelfActive(false);
    }
    if (uiData.customFlags2 & (int)UIExportFlag2::Duplicatable)
    {
        element->localUIdatasByIndex[uiData.uiIndex] = uiData;
        for (auto child : element->GetGameObject()->GetChildrenRecursively())
        {
            if (auto childElement = child->GetComponent<UIElement>())
            {
                element->localUIdatasByIndex[childElement->importedUIData.uiIndex] = childElement->importedUIData;
            }
        }
    }
}
// 특별한 로직이 적용되어야 하는 경우 참, 그렇지 않으면 거짓을 반환합니다.
bool UIManager::ImportDealWithSpecialCases(const JsonUIData& uiData, UIElement* element)
{
    if (uiData.enumID != 0)
        SetUIElementWithEnum((UIEnumID)uiData.enumID, element);
    SetUIDataWithIndex(uiData.uiIndex, uiData);
    SetUIElementWithIndex(uiData.uiIndex, element);
    switch ((UIEnumID)uiData.enumID)
    {
    case UIEnumID::Portrait_Robin:
        ImportDefaultAction(uiData, GetUIElementWithIndex(uiData.uiIndex));
        element->button->AddButtonClickFunction([=]()
            {
                InputManager::Instance().SelectPlayer(Unit::UnitType::Warrior);
            });
        break;
    case UIEnumID::Portrait_Ursula:
        ImportDefaultAction(uiData, GetUIElementWithIndex(uiData.uiIndex));
        element->button->AddButtonClickFunction([=]()
            {
                InputManager::Instance().SelectPlayer(Unit::UnitType::Magician);
            });
        break;
    case UIEnumID::Portrait_Hansel:
        ImportDefaultAction(uiData, GetUIElementWithIndex(uiData.uiIndex));
        element->button->AddButtonClickFunction([=]()
            {
                InputManager::Instance().SelectPlayer(Unit::UnitType::Healer);
            });
        break;
    case UIEnumID::Skill_Use_Q_Robin:
        ImportDefaultAction(uiData, GetUIElementWithIndex(uiData.uiIndex));
        element->button->AddButtonClickFunction([=]()
            {
                InputManager::Instance().PrepareSkill(Unit::SkillEnum::Q, Unit::UnitType::Warrior);
            });
        break;
    case UIEnumID::Skill_Use_W_Robin:
        ImportDefaultAction(uiData, GetUIElementWithIndex(uiData.uiIndex));
        element->button->AddButtonClickFunction([=]()
            {
                InputManager::Instance().PrepareSkill(Unit::SkillEnum::W, Unit::UnitType::Warrior);
            });
        break;
    case UIEnumID::Skill_Use_Q_Ursula:
        ImportDefaultAction(uiData, GetUIElementWithIndex(uiData.uiIndex));
        element->button->AddButtonClickFunction([=]()
            {
                InputManager::Instance().PrepareSkill(Unit::SkillEnum::Q, Unit::UnitType::Magician);
            });
        break;
    case UIEnumID::Skill_Use_W_Ursula:
        ImportDefaultAction(uiData, GetUIElementWithIndex(uiData.uiIndex));
        element->button->AddButtonClickFunction([=]()
            {
                InputManager::Instance().PrepareSkill(Unit::SkillEnum::W, Unit::UnitType::Magician);
            });
        break;
    case UIEnumID::Skill_Use_Q_HANSEL:
        ImportDefaultAction(uiData, GetUIElementWithIndex(uiData.uiIndex));
        element->button->AddButtonClickFunction([=]()
            {
                InputManager::Instance().PrepareSkill(Unit::SkillEnum::Q, Unit::UnitType::Healer);
            });
        break;
    case UIEnumID::Skill_Use_W_HANSEL:
        ImportDefaultAction(uiData, GetUIElementWithIndex(uiData.uiIndex));
        element->button->AddButtonClickFunction([=]()
            {
                InputManager::Instance().PrepareSkill(Unit::SkillEnum::W, Unit::UnitType::Healer);
            });
        break;
    case UIEnumID::Toggle_TacticMode:
        ImportDefaultAction(uiData, GetUIElementWithIndex(uiData.uiIndex));
        element->button->AddButtonClickFunction([=]()
            {
                InputManager::Instance().ToggleTacticMode();
            });
        break;
    case UIEnumID::PopUpMessage_PermissionForUpgradeProceedButton:
        ImportDefaultAction(uiData, GetUIElementWithIndex(uiData.uiIndex));
        element->button->AddButtonClickFunction([=]()
            {
                SkillUpgradeSystem::SingleInstance().UpgradeSkill();
            });
        break;
    default:
        return false;
        break;
    }
    return true;
}
// 아래 두 함수들을 응용해 UI들이 다 생성되고 난 후 추가적인 작업을 수행합니다.
bool UIManager::ImportDealWithSpecialCases_Post(const JsonUIData& uiData, UIElement* element)
{
    switch ((UIEnumID)uiData.enumID)
    {
    default:
        return false;
        break;
    }
    return true;
}
