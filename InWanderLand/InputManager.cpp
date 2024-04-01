#include "InWanderLand.h"
#include "InputManager.h"
#include "PlayerController.h"
#include "TacticModeSystem.h"
#include "Unit.h"
#include "ContentsLayer.h"
#include "Application.h"

void InputManager::Start()
{
	application::contents::ContentsLayer* contentsLayer = dynamic_cast<application::contents::ContentsLayer*>(application::Application::GetInstance().GetContentsLayer());
	contentsLayer->RegisterToEditorComponentVector(this);
}

void InputManager::Update()
{
	if (!isMouseOnUIButton)
	{
		if (yunutyEngine::Input::isKeyPushed(KeyCode::NUM_1))
		{
			PlayerController::SingleInstance().SetCurrentPlayerSerialNumber(Unit::UnitType::Warrior);
			currentSelectedSerialNumber = SelectedSerialNumber::One;
			isPlayerSelected = true;
			SkillPreviewSystem::Instance().ActivateSkillPreview(false);
		}
		if (yunutyEngine::Input::isKeyPushed(KeyCode::NUM_2))
		{
			PlayerController::SingleInstance().SetCurrentPlayerSerialNumber(Unit::UnitType::Magician);
			currentSelectedSerialNumber = SelectedSerialNumber::Two;
			isPlayerSelected = true;
			SkillPreviewSystem::Instance().ActivateSkillPreview(false);
		}
		if (yunutyEngine::Input::isKeyPushed(KeyCode::NUM_3))
		{
			PlayerController::SingleInstance().SetCurrentPlayerSerialNumber(Unit::UnitType::Healer);
			currentSelectedSerialNumber = SelectedSerialNumber::Three;
			isPlayerSelected = true;
			SkillPreviewSystem::Instance().ActivateSkillPreview(false);
		}

		//if (yunutyEngine::Input::isKeyPushed(KeyCode::NUM_4))
		//{
		//	PlayerController::SingleInstance().SetCurrentPlayerSerialNumber(Unit::UnitType::AllPlayers);
		//	currentSelectedSerialNumber = SelectedSerialNumber::All;
		//	isPlayerSelected = true;
		//	SkillPreviewSystem::Instance().ActivateSkillPreview(false);
		//}

		if (isPlayerSelected)
		{
			if (yunutyEngine::Input::isKeyPushed(KeyCode::A))
			{
				if (tacticMode)
					TacticModeSystem::SingleInstance().SetLeftClickAddQueueForAttackMove(currentSelectedSerialNumber);
				else
					PlayerController::SingleInstance().SetLeftClickAttackMove();
			}

			if (yunutyEngine::Input::isKeyPushed(KeyCode::Q))
			{
				if (tacticMode)
					TacticModeSystem::SingleInstance().SetLeftClickAddQueueForSkill(currentSelectedSerialNumber, Unit::SkillEnum::Q);
				else
					PlayerController::SingleInstance().SetLeftClickSkill(Unit::SkillEnum::Q);

				/// Skill Preview Area

			}

			if (yunutyEngine::Input::isKeyPushed(KeyCode::Alphabet_E))
			{
				if (tacticMode)
					TacticModeSystem::SingleInstance().SetLeftClickAddQueueForSkill(currentSelectedSerialNumber, Unit::SkillEnum::W);
				else
					PlayerController::SingleInstance().SetLeftClickSkill(Unit::SkillEnum::W);
			}
		}

		if (yunutyEngine::Input::isKeyPushed(KeyCode::Space))
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
