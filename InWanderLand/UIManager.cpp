#include "UIManager.h"
#include "RTSCam.h"
#include "UIManager.h"
#include "UIImage.h"
#include "InWanderLand.h"
#include "PopupOnEnable.h"
#include "ContentsLayer.h"
#include "UIElement.h"
#include <fstream>

void UIManager::Clear()
{
    m_highestPriorityButton = nullptr;
}

void UIManager::ReportButtonOnMouse(UIButton* p_btn)
{
    // 2. Layer에 따라 현재 상호작용할 버튼을 정합니다.
    if (m_currentHighestLayer < p_btn->GetLayer())
    {
        if (m_highestPriorityButton != nullptr)				// 이전에 선택됐던 버튼의 이미지를 Idle로 초기화
        {
            m_highestPriorityButton->m_ImageComponent->GetGI().SetImage(m_highestPriorityButton->m_IdleImage);
        }

        /// highestPriorityButton 재정의
        m_currentHighestLayer = p_btn->GetLayer();
        m_highestPriorityButton = p_btn;
    }

    // 3. 멤버 리스트에 버튼을 담아줍니다.
    m_currentSelectedButtonList.push_back(p_btn);

    /// 4. 현재 상호작용할 버튼의 click함수로 leftClick함수를 재정의합니다.
    // 마우스를 올렸을 때 호출되는 함수도 호출합니다.
    if (m_highestPriorityButton != nullptr && m_highestPriorityButton->m_onMouseFunction != nullptr)
        m_highestPriorityButton->m_onMouseFunction();

    /// 버튼 클릭이 가능하도록 bool값을 켜줍니다.
    isButtonActiviated = true;
}

void UIManager::ReportMouseExitButton(UIButton* p_btn)
{
    p_btn->m_ImageComponent->GetGI().SetImage(p_btn->m_IdleImage);
    m_currentSelectedButtonList.remove(p_btn);

    /// 벗어난 후 마우스가 ui 위에 존재하지 않는다면...
    if (m_currentSelectedButtonList.empty())
    {
        if (m_highestPriorityButton && m_highestPriorityButton->m_onMouseExitFunction)
        {
            m_highestPriorityButton->m_onMouseExitFunction();
        }
        m_highestPriorityButton = nullptr;
        m_currentHighestLayer = -1;
        isButtonActiviated = false;
    }
    /// 만약, 마우스가 벗어난 버튼이 Highest-Priority였다면 leftClick 함수를 재정의합니다.
    // 그러기 위해선, 다음 우선순위(layer)의 버튼을 재정의 해줘야합니다.
    else
    {
        if (p_btn == m_highestPriorityButton)
        {
            m_currentHighestLayer = -1;						// 초기화
            if (m_highestPriorityButton->m_onMouseExitFunction)
            {
                m_highestPriorityButton->m_onMouseExitFunction();
            }
            for (auto e : m_currentSelectedButtonList)
            {
                if (e->GetLayer() > m_currentHighestLayer)
                {
                    m_currentHighestLayer = e->GetLayer();
                    m_highestPriorityButton = e;
                }
            }

            m_highestPriorityButton->m_onMouseFunction();

            isButtonActiviated = true;
        }
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
    return nullptr;
}

void UIManager::Update()
{
    /*if (m_currentSelectedButtonList.empty())
    {
        InputManager::Instance().IsMouseOnUI(false);
    }
    else
    {
        InputManager::Instance().IsMouseOnUI(true);
    }*/

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
            uisByEnumID[(UIEnumID)uiData.enumID] = uiElement;
            uisByName[uiData.uiname] = uiElement;
            uidatasByName[uiData.uiname] = uiData;


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
            uidatasByName[uiData.uiname] = uiData;
            if (ImportDealWithSpecialCases_Post(uiData, uisByName[uiData.uiname]) == false)
            {
                ImportDefaultAction_Post(uiData, uisByName[uiData.uiname]);
            }
            uiImportingPriority++;
        }
    }
}

// JsonUIData만으로 UI를 생성합니다.
void UIManager::ImportDefaultAction(const UIManager::JsonUIData& uiData, UIElement* element)
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
        uiButtonComponent = element->button = uiObject->AddComponent<UIButton>();
        uiButtonComponent->SetImageComponent(uiImageComponent);
        uiButtonComponent->SetIdleImage(idleTexture);
        uiButtonComponent->SetOnMouseImage(rsrcMgr->GetTexture(L"Texture/zoro.jpg"));
    }
    // 만약 버튼이라면...
    if (uiData.customFlags & (int)UIExportFlag::IsButton)
    {
        uiButtonComponent->SetOnMouseImage(rsrcMgr->GetTexture(L"Texture/zoro.jpg"));
    }

    Vector3d pivotPos{ 0,0,0 };
    // offset by anchor
    Vector3d parentSize{ 1920, 1080, 0 };
    Vector2d parentPivot{ 0.0,0.0 };
    if (uiData.parentUIName != "")
    {
        auto parentData = uidatasByName[uiData.parentUIName];
        auto parent = uisByName[uiData.parentUIName]->GetGameObject();
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
void UIManager::ImportDefaultAction_Post(const UIManager::JsonUIData& uiData, UIElement* element)
{
    UIButton* button{ element->button };
    // 만약 닫기 버튼이라면...
    if (uiData.customFlags & (int)UIExportFlag::CloseButton)
    {
        button->SetButtonClickFunction([=]()
            {
                if (auto parent = button->GetGameObject()->GetParentGameObject())
                {
                    parent->SetSelfActive(false);
                }
            });
    }
    // 만약 열기 버튼이라면...
    if (uiData.customFlags & (int)UIExportFlag::OpeningButton)
    {
        auto openTarget = uisByName[uiData.openTarget];
        assert(openTarget);
        button->SetButtonClickFunction([=]()
            {
                openTarget->GetGameObject()->SetSelfActive(true);
            });
    }
    // 만약 툴팁을 포함하는 UI라면...
    if (uiData.customFlags & (int)UIExportFlag::IsIncludingTooltips)
    {
        for (auto each : button->GetGameObject()->GetChildren())
        {
            each->SetSelfActive(false);
        }
        button->m_OnMouseEventFunction = [=]()
            {
                for (auto each : button->GetGameObject()->GetChildren())
                {
                    each->SetSelfActive(true);
                    if (auto img = each->GetComponent<UIImage>())
                    {
                        img->GetGI().SetLayer(123456789);
                    }
                }
            };
        button->m_onMouseExitFunction = [=]()
            {
                for (auto each : button->GetGameObject()->GetChildren())
                {
                    each->SetSelfActive(false);
                }
            };
    }
}
// 특별한 로직이 적용되어야 하는 경우 참, 그렇지 않으면 거짓을 반환합니다.
bool UIManager::ImportDealWithSpecialCases(const UIManager::JsonUIData& uiData, UIElement* element)
{
    switch ((UIEnumID)uiData.enumID)
    {
    case UIEnumID::Portrait_Robin:
        ImportDefaultAction(uiData, uisByName[uiData.uiname]);
        element->button->SetButtonClickFunction([=]()
            {
                InputManager::Instance().SelectPlayer(Unit::UnitType::Warrior);
            });
        break;
    case UIEnumID::Portrait_Ursula:
        ImportDefaultAction(uiData, uisByName[uiData.uiname]);
        element->button->SetButtonClickFunction([=]()
            {
                InputManager::Instance().SelectPlayer(Unit::UnitType::Magician);
            });
        break;
    case UIEnumID::Portrait_Hansel:
        ImportDefaultAction(uiData, uisByName[uiData.uiname]);
        element->button->SetButtonClickFunction([=]()
            {
                InputManager::Instance().SelectPlayer(Unit::UnitType::Healer);
            });
        break;
    case UIEnumID::Skill_Use_Q_Robin:
        ImportDefaultAction(uiData, uisByName[uiData.uiname]);
        element->button->SetButtonClickFunction([=]()
            {
                InputManager::Instance().PrepareSkill(Unit::SkillEnum::Q, Unit::UnitType::Warrior);
            });
        break;
    case UIEnumID::Skill_Use_W_Robin:
        ImportDefaultAction(uiData, uisByName[uiData.uiname]);
        element->button->SetButtonClickFunction([=]()
            {
                InputManager::Instance().PrepareSkill(Unit::SkillEnum::W, Unit::UnitType::Warrior);
            });
        break;
    case UIEnumID::Skill_Use_Q_Ursula:
        ImportDefaultAction(uiData, uisByName[uiData.uiname]);
        element->button->SetButtonClickFunction([=]()
            {
                InputManager::Instance().PrepareSkill(Unit::SkillEnum::Q, Unit::UnitType::Magician);
            });
        break;
    case UIEnumID::Skill_Use_W_Ursula:
        ImportDefaultAction(uiData, uisByName[uiData.uiname]);
        element->button->SetButtonClickFunction([=]()
            {
                InputManager::Instance().PrepareSkill(Unit::SkillEnum::W, Unit::UnitType::Magician);
            });
        break;
    case UIEnumID::Skill_Use_Q_HANSEL:
        ImportDefaultAction(uiData, uisByName[uiData.uiname]);
        element->button->SetButtonClickFunction([=]()
            {
                InputManager::Instance().PrepareSkill(Unit::SkillEnum::Q, Unit::UnitType::Healer);
            });
        break;
    case UIEnumID::Skill_Use_W_HANSEL:
        ImportDefaultAction(uiData, uisByName[uiData.uiname]);
        element->button->SetButtonClickFunction([=]()
            {
                InputManager::Instance().PrepareSkill(Unit::SkillEnum::W, Unit::UnitType::Healer);
            });
        break;
    default:
        return false;
        break;
    }
    return true;
}
// 아래 두 함수들을 응용해 UI들이 다 생성되고 난 후 추가적인 작업을 수행합니다.
bool UIManager::ImportDealWithSpecialCases_Post(const UIManager::JsonUIData& uiData, UIElement* element)
{
    switch ((UIEnumID)uiData.enumID)
    {
    default:
        return false;
        break;
    }
    return true;
}
