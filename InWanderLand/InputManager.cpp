#include "InWanderLand.h"
#include "InputManager.h"
#include "PlayerController.h"
#include "TacticModeSystem.h"
#include "Unit.h"

void InputManager::Start()
{

}

void InputManager::Update()
{
	if (yunutyEngine::Input::isKeyPushed(KeyCode::NUM_1))
	{
		PlayerController::GetInstance()->SetCurrentPlayerSerialNumber(Unit::UnitType::Warrior);
		currentSelectedSerialNumber = SelectedSerialNumber::One;
	}
	if (yunutyEngine::Input::isKeyPushed(KeyCode::NUM_2))
	{
		PlayerController::GetInstance()->SetCurrentPlayerSerialNumber(Unit::UnitType::Magician);
		currentSelectedSerialNumber = SelectedSerialNumber::Two;
	}
	if (yunutyEngine::Input::isKeyPushed(KeyCode::NUM_3))
	{
		PlayerController::GetInstance()->SetCurrentPlayerSerialNumber(Unit::UnitType::Healer);
		currentSelectedSerialNumber = SelectedSerialNumber::Three;
	}

	if (yunutyEngine::Input::isKeyPushed(KeyCode::NUM_4))
	{
		PlayerController::GetInstance()->SetCurrentPlayerSerialNumber(Unit::UnitType::AllPlayers);
		currentSelectedSerialNumber = SelectedSerialNumber::All;
	}

	if (yunutyEngine::Input::isKeyPushed(KeyCode::A))
	{
		if (tacticMode)
			TacticModeSystem::SingleInstance().SetLeftClickAddQueueForAttackMove(currentSelectedSerialNumber);
		else
			PlayerController::GetInstance()->SetLeftClickAttackMove();
	}

	if (yunutyEngine::Input::isKeyPushed(KeyCode::Q))
	{
		if (tacticMode)
			TacticModeSystem::SingleInstance().SetLeftClickAddQueueForSkill(currentSelectedSerialNumber, Unit::SkillEnum::Q);
		else
			PlayerController::GetInstance()->SetLeftClickSkill(Unit::SkillEnum::Q);
	}

	if (yunutyEngine::Input::isKeyPushed(KeyCode::Alphabet_E))
	{
		if (tacticMode)
			TacticModeSystem::SingleInstance().SetLeftClickAddQueueForSkill(currentSelectedSerialNumber, Unit::SkillEnum::W);
		else
			PlayerController::GetInstance()->SetLeftClickSkill(Unit::SkillEnum::W);
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
