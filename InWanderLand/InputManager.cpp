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
	//currentSelectedSerialNumber = SelectedSerialNumber::One;
	//PlayerController::SingleInstance().SetCurrentPlayerSerialNumber(Unit::UnitType::Warrior);
}

void InputManager::Update()
{
	if (!isMouseOnUIButton)
	{
		if (GameManager::Instance().IsBattleSystemOperating())
		{
			if (yunutyEngine::Input::isKeyPushed(KeyCode::NUM_1))
			{
				PlayerController::SingleInstance().SetCurrentPlayerSerialNumber(Unit::UnitType::Warrior);
				rtscam->SetTarget(PlayerController::SingleInstance().GetPlayerMap().find(Unit::UnitType::Warrior)->second->GetGameObject());
				currentSelectedSerialNumber = SelectedSerialNumber::One;
				isPlayerSelected = true;
				SkillPreviewSystem::Instance().ActivateSkillPreview(false);
			}
			if (yunutyEngine::Input::isKeyPushed(KeyCode::NUM_2))
			{
				PlayerController::SingleInstance().SetCurrentPlayerSerialNumber(Unit::UnitType::Magician);
				rtscam->SetTarget(PlayerController::SingleInstance().GetPlayerMap().find(Unit::UnitType::Magician)->second->GetGameObject());
				currentSelectedSerialNumber = SelectedSerialNumber::Two;
				isPlayerSelected = true;
				SkillPreviewSystem::Instance().ActivateSkillPreview(false);
			}
			if (yunutyEngine::Input::isKeyPushed(KeyCode::NUM_3))
			{
				PlayerController::SingleInstance().SetCurrentPlayerSerialNumber(Unit::UnitType::Healer);
				rtscam->SetTarget(PlayerController::SingleInstance().GetPlayerMap().find(Unit::UnitType::Healer)->second->GetGameObject());
				currentSelectedSerialNumber = SelectedSerialNumber::Three;
				isPlayerSelected = true;
				SkillPreviewSystem::Instance().ActivateSkillPreview(false);
			}
			if (yunutyEngine::Input::isKeyPushed(KeyCode::NUM_4))
			{
				PlayerController::SingleInstance().SetCurrentPlayerSerialNumber(Unit::UnitType::AllPlayers);
				currentSelectedSerialNumber = SelectedSerialNumber::All;
				isPlayerSelected = true;
				SkillPreviewSystem::Instance().ActivateSkillPreview(false);
			}
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
				if (tacticMode)
					TacticModeSystem::SingleInstance().SetLeftClickAddQueueForSkill(currentSelectedSerialNumber, Unit::SkillEnum::Q);
				else
				{
					PlayerController::SingleInstance().SetCurrentPlayerSerialNumber(static_cast<Unit::UnitType>(currentSelectedSerialNumber));
					PlayerController::SingleInstance().SetLeftClickSkill(Unit::SkillEnum::Q);
				}
			}

			if (yunutyEngine::Input::isKeyPushed(KeyCode::W))
			{
				if (tacticMode)
					TacticModeSystem::SingleInstance().SetLeftClickAddQueueForSkill(currentSelectedSerialNumber, Unit::SkillEnum::W);
				else
				{
					PlayerController::SingleInstance().SetCurrentPlayerSerialNumber(static_cast<Unit::UnitType>(currentSelectedSerialNumber));
					PlayerController::SingleInstance().SetLeftClickSkill(Unit::SkillEnum::W);
				}
			}
		}

		if (yunutyEngine::Input::isKeyPushed(KeyCode::Space) && GameManager::Instance().IsBattleSystemOperating())
		{
			tacticMode = !tacticMode;
			if (tacticMode)
			{
				TacticModeSystem::SingleInstance().EngageTacticMode();
			}
			else
			{
				TacticModeSystem::SingleInstance().ExitTacticMode();
			}
		}
	}

}

void InputManager::IsMouseOnUI(bool p_boolen)
{
	isMouseOnUIButton = p_boolen;
}
