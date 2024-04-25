#include "InWanderLand.h"
#include "InputManager.h"
#include "PlayerController.h"
#include "TacticModeSystem.h"
#include "Unit.h"
#include "ContentsLayer.h"
#include "Application.h"
#include "RTSCam.h"
#include "GameManager.h"

void InputManager::Start()
{
    application::contents::ContentsLayer* contentsLayer = dynamic_cast<application::contents::ContentsLayer*>(application::Application::GetInstance().GetContentsLayer());
    contentsLayer->RegisterToEditorComponentVector(this);
}

void InputManager::Update()
{
    if (isInputManagerActivated)
    {
        if (!isMouseOnUIButton)
        {
            if (yunutyEngine::Input::isKeyPushed(KeyCode::NUM_1))
            {
                SelectPlayer(Unit::UnitType::Warrior);
            }
            if (yunutyEngine::Input::isKeyPushed(KeyCode::NUM_2))
            {
                SelectPlayer(Unit::UnitType::Magician);
            }
            if (yunutyEngine::Input::isKeyPushed(KeyCode::NUM_3))
            {
                SelectPlayer(Unit::UnitType::Healer);
            }
            if (yunutyEngine::Input::isKeyPushed(KeyCode::NUM_4))
            {
                SelectPlayer(Unit::UnitType::AllPlayers);
            }

            if (isPlayerSelected && GameManager::Instance().IsBattleSystemOperating() && currentSelectedSerialNumber != All)
            {
                if (yunutyEngine::Input::isKeyPushed(KeyCode::A))
                {
                    if (tacticMode)
                        TacticModeSystem::SingleInstance().SetLeftClickAddQueueForAttackMove(currentSelectedSerialNumber);
                    else
                    {
                        PlayerController::SingleInstance().SetCurrentPlayerSerialNumber(static_cast<Unit::UnitType>(currentSelectedSerialNumber));
                        PlayerController::SingleInstance().SetLeftClickAttackMove();
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
        PlayerController::SingleInstance().FindSelectedUnitByUnitType(p_unitType)->GetCurrentUnitState() == Unit::UnitState::Death)
    {
        return;
    }
    PlayerController::SingleInstance().SetCurrentPlayerSerialNumber(p_unitType);
    if (p_unitType != Unit::UnitType::AllPlayers)
    {
        rtscam->SetTarget(PlayerController::SingleInstance().GetPlayerMap().find(p_unitType)->second->GetGameObject());
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
    
    if (tacticMode)
    {
		TacticModeSystem::SingleInstance().SetLeftClickAddQueueForMove(currentSelectedSerialNumber);
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
        PlayerController::SingleInstance().FindSelectedUnitByUnitType(static_cast<Unit::UnitType>(currentSelectedSerialNumber))->GetCurrentUnitState() == Unit::UnitState::Death )
    {
        return;
    }
    if (tacticMode)
    {
        TacticModeSystem::SingleInstance().SetLeftClickAddQueueForSkill(currentSelectedSerialNumber, p_skillType);
    }
    else
    {
        PlayerController::SingleInstance().SetCurrentPlayerSerialNumber(static_cast<Unit::UnitType>(currentSelectedSerialNumber));
        PlayerController::SingleInstance().SetLeftClickSkill(p_skillType);
    }
}
void InputManager::ToggleTacticMode()
{
    if (GameManager::Instance().IsBattleSystemOperating())
    {
        tacticMode = !tacticMode;
        if (tacticMode)
        {
            TacticModeSystem::SingleInstance().EngageTacticMode();
			TacticModeSystem::SingleInstance().SetLeftClickAddQueueForMove(currentSelectedSerialNumber);
        }
        else
        {
            TacticModeSystem::SingleInstance().ExitTacticMode();
        }
    }
}
