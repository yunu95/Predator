#include "InWanderLand.h"
#include "InputManager.h"
#include "PlayerController.h"
#include "TacticModeSystem.h"
#include "Unit.h"
#include "RTSCam.h"
#include "GameManager.h"
#include "PlayerSkillManager.h"

void InputManager::Start()
{
}

void InputManager::Update()
{
    if (isInputManagerActivated)
    {
        if (!isMouseOnUIButton)
        {
            if (yunutyEngine::Input::isKeyPushed(KeyCode::NUM_1))
            {
                UIManager::Instance().GetUIElementByEnum(UIEnumID::CharInfo_Robin)->
                    GetLocalUIsByEnumID().at(UIEnumID::CharInfo_Portrait)->button->InvokeButtonClickEvent();
            }
            if (yunutyEngine::Input::isKeyPushed(KeyCode::NUM_2))
            {
                UIManager::Instance().GetUIElementByEnum(UIEnumID::CharInfo_Ursula)->
                    GetLocalUIsByEnumID().at(UIEnumID::CharInfo_Portrait)->button->InvokeButtonClickEvent();
            }
            if (yunutyEngine::Input::isKeyPushed(KeyCode::NUM_3))
            {
                UIManager::Instance().GetUIElementByEnum(UIEnumID::CharInfo_Hansel)->
                    GetLocalUIsByEnumID().at(UIEnumID::CharInfo_Portrait)->button->InvokeButtonClickEvent();
            }
            if (yunutyEngine::Input::isKeyPushed(KeyCode::NUM_4))
            {
                SelectPlayer(Unit::UnitType::AllPlayers);
            }

            if (isPlayerSelected && GameManager::Instance().IsBattleSystemOperating() && currentSelectedSerialNumber != All)
            {
                if (yunutyEngine::Input::isKeyPushed(KeyCode::A))
                {
                    /*if (TacticModeSystem::Instance().IsOrderingTimingNow())
                        TacticModeSystem::Instance().SetLeftClickAddQueueForAttackMove(currentSelectedSerialNumber);
                    else*/
                    {
                        PlayerController::Instance().SetCurrentPlayerSerialNumber(static_cast<Unit::UnitType>(currentSelectedSerialNumber));
                        PlayerController::Instance().SetLeftClickAttackMove();
                    }
                    SkillPreviewSystem::Instance().ActivateSkillPreview(false);
                }

                if (yunutyEngine::Input::isKeyPushed(KeyCode::Q))
                {
                    PrepareSkill(Unit::SkillEnum::Q);
                }

                if (yunutyEngine::Input::isKeyPushed(KeyCode::Alphabet_E))
                {
                    PrepareSkill(Unit::SkillEnum::W);
                }
            }

            if (yunutyEngine::Input::isKeyPushed(KeyCode::Space) && GameManager::Instance().IsBattleSystemOperating())
            {
                ToggleTacticMode();
            }
        }
    }
}

void InputManager::PlayFunction()
{
    this->SetActive(true);
    if (isOncePaused)
    {
        Start();
    }
}

void InputManager::StopFunction()
{
    this->SetActive(false);

    rtscam = nullptr;

    currentSelectedSerialNumber = SelectedSerialNumber::One;
    isPlayerSelected = false;
    //tacticMode = false;
    isMouseOnUIButton = false;
    isInputManagerActivated = false;
}

bool InputManager::GetInputManagerActive()
{
    return isInputManagerActivated;
}
void InputManager::SetInputManagerActive(bool p_boolen)
{
    isInputManagerActivated = p_boolen;
}

void InputManager::SelectPlayer(Unit::UnitType p_unitType)
{
    if (!GameManager::Instance().IsBattleSystemOperating() ||
        PlayerController::Instance().FindSelectedUnitByUnitType(p_unitType)->GetCurrentUnitState() == Unit::UnitState::Death)
    {
        return;
    }

    PlayerController::Instance().SetCurrentPlayerSerialNumber(p_unitType);

    if (p_unitType != Unit::UnitType::AllPlayers)
    {
        rtscam->SetTarget(PlayerController::Instance().GetPlayerMap().find(p_unitType)->second->GetGameObject());
    }
    switch (p_unitType)
    {
    case Unit::UnitType::Warrior:
        currentSelectedSerialNumber = SelectedSerialNumber::One;
        break;
    case Unit::UnitType::Magician:
        currentSelectedSerialNumber = SelectedSerialNumber::Two;
        break;
    case Unit::UnitType::Healer:
        currentSelectedSerialNumber = SelectedSerialNumber::Three;
        break;
    case Unit::UnitType::AllPlayers:
        currentSelectedSerialNumber = SelectedSerialNumber::All;
        break;
    default:
        break;
    }
    isPlayerSelected = true;

    if (TacticModeSystem::Instance().IsOrderingTimingNow())
    {
        TacticModeSystem::Instance().SetTacticModeRightClickFunction(currentSelectedSerialNumber);
    }

    SkillPreviewSystem::Instance().ActivateSkillPreview(false);
}

void InputManager::PrepareSkill(Unit::SkillEnum p_skillType, Unit::UnitType p_unitType)
{
    SelectPlayer(p_unitType);
    PrepareSkill(p_skillType);
}
void InputManager::PrepareSkill(Unit::SkillEnum p_skillType)
{
    if (!GameManager::Instance().IsBattleSystemOperating() ||
        PlayerController::Instance().FindSelectedUnitByUnitType(static_cast<Unit::UnitType>(currentSelectedSerialNumber))->GetCurrentUnitState() == Unit::UnitState::Death)
    {
        return;
    }
    if (!PlayerSkillManager::Instance().IsSkillGaugeEnoughToBeUsed(static_cast<Unit::UnitType>(currentSelectedSerialNumber), p_skillType))
    {
        /// 마나가 부족해 스킬을 사용하지 못하는 경우입니다.
        UIManager::Instance().GetUIElementByEnum(UIEnumID::ErrorPopup_NoMana)->EnableElement();
        return;
    }
    if (TacticModeSystem::Instance().IsOrderingTimingNow())
    {
        TacticModeSystem::Instance().SetLeftClickAddQueueForSkill(currentSelectedSerialNumber, p_skillType);
    }
    else
    {
        PlayerController::Instance().SetCurrentPlayerSerialNumber(static_cast<Unit::UnitType>(currentSelectedSerialNumber));
        PlayerController::Instance().SetLeftClickSkill(p_skillType);
    }
}
void InputManager::ToggleTacticMode()
{
    if (GameManager::Instance().IsBattleSystemOperating())
    {
        TacticModeSystem::Instance().ToggleRequested(currentSelectedSerialNumber);

 /*       if (tacticMode == false && !TacticModeSystem::Instance().IsTacticModeCoolTime())
            tacticMode = true;

        if (tacticMode)
        {
            TacticModeSystem::Instance().EngageTacticMode();
            TacticModeSystem::Instance().SetTacticModeRightClickFunction(currentSelectedSerialNumber);
        }
        else
        {
            TacticModeSystem::Instance().ExitTacticMode();
            tacticMode = false;
        }*/
    }
}
