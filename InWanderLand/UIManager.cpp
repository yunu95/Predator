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
#include <fstream>

using namespace yunutyEngine::graphics;
void UIManager::Clear()
{
    m_highestPriorityButton = nullptr;
    m_currentHighestLayer = -1;
    m_selectedButtons.clear();
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
        UIManager::Instance().GetUIElementByEnum(UIEnumID::Ingame_Combo)->EnableElement();
        UIManager::Instance().GetUIElementByEnum(UIEnumID::Ingame_Combo_Description)->EnableElement();
    }
    else
    {
        UIManager::Instance().GetUIElementByEnum(UIEnumID::Ingame_Bottom_Layout)->DisableElement();
        UIManager::Instance().GetUIElementByEnum(UIEnumID::Ingame_MenuButton)->DisableElement();
        UIManager::Instance().GetUIElementByEnum(UIEnumID::Ingame_Combo)->DisableElement();
        UIManager::Instance().GetUIElementByEnum(UIEnumID::Ingame_Combo_Description)->DisableElement();
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
void UIManager::UpdateHighestPriorityButton()
{
    // 현재 하이라이트된 버튼이 적법한 버튼이라면, 더 이상의 처리는 필요없습니다.
    if (!m_selectedButtons.empty() && m_highestPriorityButton == *m_selectedButtons.begin())
        return;
    // 하이라이트된 버튼이 바뀌어야 한다면, 기존 버튼의 Exit 이벤트부터 처리합니다.
    if (m_highestPriorityButton)
    {
        m_highestPriorityButton->m_ImageComponent->GetGI().SetImage(m_highestPriorityButton->m_IdleImage);
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
        m_highestPriorityButton->m_onMouseFunction();
    }
}

bool UIManager::IsMouseOnButton()
{
    return isButtonActiviated;
}
UIElement* UIManager::GetUIElementByEnum(UIEnumID uiEnumID)
{
    if (uisByEnumID.find(uiEnumID) != uisByEnumID.end())
        return uisByEnumID[uiEnumID];
    assert(false);
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
            if (uiData.enumID != 0)
                uisByEnumID[(UIEnumID)uiData.enumID] = uiElement;
            uisByIndex[uiData.uiIndex] = uiElement;
            uidatasByIndex[uiData.uiIndex] = uiData;

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
            uidatasByIndex[uiData.uiIndex] = uiData;
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
    if (uiData.imagePath != "")
    {
        uiImageComponent = element->imageComponent = uiObject->AddComponent<UIImage>();
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
            uiButtonComponent->SetIdleImage(idleTexture);
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
        element->imageComponent->GetGI().SetRadialFillMode(true);
        element->imageComponent->GetGI().SetRadialFillDegree(0);
        element->adjuster = uiObject->AddComponent<FloatFollower>();
        element->adjuster->SetFollowingRate(uiData.adjustingRate);
        element->adjuster->applier = [=](float val)
            {
                element->imageComponent->GetGI().SetRadialFillDegree(val * 360);
            };
    }

    Vector3d pivotPos{ 0,0,0 };
    // offset by anchor
    Vector3d parentSize{ 1920, 1080, 0 };
    Vector2d parentPivot{ 0.0,0.0 };
    if (uiData.parentUIIndex != -1)
    {
        auto parentData = uidatasByIndex[uiData.parentUIIndex];
        auto parent = uisByIndex[uiData.parentUIIndex]->GetGameObject();
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
    // 만약 닫기 버튼이라면...
    if (uiData.customFlags & (int)UIExportFlag::CloseButton)
    {
        button->AddButtonClickFunction([=]()
            {
                if (auto parent = button->GetGameObject()->GetParentGameObject())
                {
                    if (auto elem = parent->GetComponent<UIElement>())
                    {
                        elem->DisableElement();
                    }
                }
            });
    }
    // 만약 열기 버튼이라면...
    if (uiData.customFlags & (int)UIExportFlag::OpeningButton)
    {
        auto openTarget = uisByIndex[uiData.openTarget];
        assert(openTarget);
        if (uiData.customFlags & (int)UIExportFlag::IsToggle)
        {
            button->AddButtonClickFunction([=]()
                {
                    if (!openTarget->GetGameObject()->GetSelfActive())
                    {
                        openTarget->EnableElement();
                    }
                    else
                    {
                        openTarget->DisableElement();
                    }
                });
        }
        else
        {
            button->AddButtonClickFunction([=]()
                {
                    openTarget->EnableElement();
                });
        }
    }
    // 만약 Disabling button이라면...
    if (uiData.customFlags & (int)UIExportFlag::DiablingButton)
    {
        auto disablingTarget = uisByIndex[uiData.disablingTargetIndex];
        assert(disablingTarget);
        button->AddButtonClickFunction([=]()
            {
                disablingTarget->DisableElement();
            });
    }
    static constexpr int priority_Tooltip = 123456789;
    // 만약 툴팁을 포함하는 UI라면...
    if (uiData.customFlags & (int)UIExportFlag::IsIncludingTooltips)
    {
        for (auto each : button->GetGameObject()->GetChildren())
        {
            each->SetSelfActive(false);
            if (auto img = each->GetComponent<UIImage>())
            {
                img->GetGI().SetLayer(priority_Tooltip);
            }
        }
        button->m_OnMouseEventFunction = [=]()
            {
                for (auto each : button->GetGameObject()->GetChildren())
                {
                    if (auto elem = each->GetComponent<UIElement>())
                    {
                        elem->EnableElement();
                    }
                }
            };
        button->m_onMouseExitFunction = [=]()
            {
                for (auto each : button->GetGameObject()->GetChildren())
                {
                    if (auto elem = each->GetComponent<UIElement>())
                    {
                        elem->DisableElement();
                    }
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
    // 초기 상태가 비활성화 상태라면...
    if (uiData.customFlags & (int)UIExportFlag::DisableOnStart)
    {
        element->GetGameObject()->SetSelfActive(false);
    }
}
// 특별한 로직이 적용되어야 하는 경우 참, 그렇지 않으면 거짓을 반환합니다.
bool UIManager::ImportDealWithSpecialCases(const JsonUIData& uiData, UIElement* element)
{
    switch ((UIEnumID)uiData.enumID)
    {
    case UIEnumID::Portrait_Robin:
        ImportDefaultAction(uiData, uisByIndex[uiData.uiIndex]);
        element->button->AddButtonClickFunction([=]()
            {
                InputManager::Instance().SelectPlayer(Unit::UnitType::Warrior);
            });
        break;
    case UIEnumID::Portrait_Ursula:
        ImportDefaultAction(uiData, uisByIndex[uiData.uiIndex]);
        element->button->AddButtonClickFunction([=]()
            {
                InputManager::Instance().SelectPlayer(Unit::UnitType::Magician);
            });
        break;
    case UIEnumID::Portrait_Hansel:
        ImportDefaultAction(uiData, uisByIndex[uiData.uiIndex]);
        element->button->AddButtonClickFunction([=]()
            {
                InputManager::Instance().SelectPlayer(Unit::UnitType::Healer);
            });
        break;
    case UIEnumID::Skill_Use_Q_Robin:
        ImportDefaultAction(uiData, uisByIndex[uiData.uiIndex]);
        element->button->AddButtonClickFunction([=]()
            {
                InputManager::Instance().PrepareSkill(Unit::SkillEnum::Q, Unit::UnitType::Warrior);
            });
        break;
    case UIEnumID::Skill_Use_W_Robin:
        ImportDefaultAction(uiData, uisByIndex[uiData.uiIndex]);
        element->button->AddButtonClickFunction([=]()
            {
                InputManager::Instance().PrepareSkill(Unit::SkillEnum::W, Unit::UnitType::Warrior);
            });
        break;
    case UIEnumID::Skill_Use_Q_Ursula:
        ImportDefaultAction(uiData, uisByIndex[uiData.uiIndex]);
        element->button->AddButtonClickFunction([=]()
            {
                InputManager::Instance().PrepareSkill(Unit::SkillEnum::Q, Unit::UnitType::Magician);
            });
        break;
    case UIEnumID::Skill_Use_W_Ursula:
        ImportDefaultAction(uiData, uisByIndex[uiData.uiIndex]);
        element->button->AddButtonClickFunction([=]()
            {
                InputManager::Instance().PrepareSkill(Unit::SkillEnum::W, Unit::UnitType::Magician);
            });
        break;
    case UIEnumID::Skill_Use_Q_HANSEL:
        ImportDefaultAction(uiData, uisByIndex[uiData.uiIndex]);
        element->button->AddButtonClickFunction([=]()
            {
                InputManager::Instance().PrepareSkill(Unit::SkillEnum::Q, Unit::UnitType::Healer);
            });
        break;
    case UIEnumID::Skill_Use_W_HANSEL:
        ImportDefaultAction(uiData, uisByIndex[uiData.uiIndex]);
        element->button->AddButtonClickFunction([=]()
            {
                InputManager::Instance().PrepareSkill(Unit::SkillEnum::W, Unit::UnitType::Healer);
            });
        break;
    case UIEnumID::Toggle_TacticMode:
        ImportDefaultAction(uiData, uisByIndex[uiData.uiIndex]);
        element->button->AddButtonClickFunction([=]()
            {
                InputManager::Instance().ToggleTacticMode();
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
