#include "InWanderLand.h"
#include "InputManager.h"
#include "PlayerController.h"
#include "TacticModeSystem.h"
#include "Unit.h"
#include "RTSCam.h"
#include "GameManager.h"
#include "PlayerSkillManager.h"

InputManager::InputManager()
{
    canPrepareSkill[Unit::UnitType::Warrior][Unit::SkillEnum::Q] = true;
    canPrepareSkill[Unit::UnitType::Warrior][Unit::SkillEnum::W] = true;
    canPrepareSkill[Unit::UnitType::Magician][Unit::SkillEnum::Q] = true;
    canPrepareSkill[Unit::UnitType::Magician][Unit::SkillEnum::W] = true;
    canPrepareSkill[Unit::UnitType::Healer][Unit::SkillEnum::Q] = true;
    canPrepareSkill[Unit::UnitType::Healer][Unit::SkillEnum::W] = true;
}
void InputManager::Start()
{
	currentSelectedSerialNumber = SelectedSerialNumber::One;
	UIManager::Instance().GetUIElementByEnum(UIEnumID::CharInfo_Robin)->
		GetLocalUIsByEnumID().at(UIEnumID::CharInfo_Portrait)->button->InvokeButtonClickEvent();
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
					//SkillPreviewSystem::Instance().ActivateSkillPreview(false);
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

			//if (yunutyEngine::Input::isKeyPushed(KeyCode::Space) && GameManager::Instance().IsBattleSystemOperating())
			//{
			//	ToggleTacticMode();
			//}

			//if (yunutyEngine::Input::isKeyDown(KeyCode::Control) 
			//	&& yunutyEngine::Input::isKeyPushed(KeyCode::Z)
			//	&& GameManager::Instance().IsBattleSystemOperating())
			//{
			//	TacticModeSystem::Instance().PopCommand();
			//}

			//if (yunutyEngine::Input::isKeyDown(KeyCode::Control)
			//	&& yunutyEngine::Input::isKeyPushed(KeyCode::X)
			//	&& GameManager::Instance().IsBattleSystemOperating())
			//{
			//	TacticModeSystem::Instance().ClearCommand();
			//}
		}
	}
}

void InputManager::OnContentsStop()
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
	if (PlayerController::Instance().FindSelectedUnitByUnitType(p_unitType)->GetCurrentUnitState() == Unit::UnitState::Death)
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

	//if (TacticModeSystem::Instance().IsOrderingTimingNow())
	//{
	//	TacticModeSystem::Instance().SetTacticModeRightClickFunction(currentSelectedSerialNumber);
	//}
}

void InputManager::PrepareSkill(Unit::SkillEnum p_skillType, Unit::UnitType p_unitType)
{
    if (!canPrepareSkill[p_unitType][p_skillType])
    {
        return;
    }
    SelectPlayer(p_unitType);
    PrepareSkill(p_skillType);
}
void InputManager::PrepareSkill(Unit::SkillEnum p_skillType)
{
    if (!canPrepareSkill[PlayerController::Instance().GetCurrentSelectedPlayerUnit()->GetUnitType()][p_skillType])
    {
        return;
    }
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
    if (!PlayerSkillManager::Instance().IsSkillCoolingDown(static_cast<Unit::UnitType>(currentSelectedSerialNumber), p_skillType))
    {
        /// 스킬이 아직 쿨타임이라 스킬을 못하는 경우.
        return;
    }
    if (/*TacticModeSystem::Instance().IsOrderingTimingNow()*/false)
    {
        //TacticModeSystem::Instance().SetLeftClickAddQueueForSkill(currentSelectedSerialNumber, p_skillType);
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
		//TacticModeSystem::Instance().ToggleRequested(currentSelectedSerialNumber);
	}
}
