#include "InputManager.h"
#include "PlayerController.h"
#include "Unit.h"

void InputManager::Start()
{

}

void InputManager::Update()
{
	if (yunutyEngine::Input::isKeyDown(KeyCode::NUM_1))
	{
		PlayerController::GetInstance()->SetCurrentPlayerSerialNumber(SelectedSerialNumber::One);
	}
	if (yunutyEngine::Input::isKeyDown(KeyCode::NUM_2))
	{
		PlayerController::GetInstance()->SetCurrentPlayerSerialNumber(SelectedSerialNumber::Two);
	}
	if (yunutyEngine::Input::isKeyDown(KeyCode::NUM_3))
	{
		PlayerController::GetInstance()->SetCurrentPlayerSerialNumber(SelectedSerialNumber::Three);
	}

	if (yunutyEngine::Input::isKeyDown(KeyCode::NUM_4))
	{
		PlayerController::GetInstance()->SetCurrentPlayerSerialNumber(SelectedSerialNumber::All);
	}

	if (yunutyEngine::Input::isKeyDown(KeyCode::A))
	{
		PlayerController::GetInstance()->SetLeftClickAttackMove();
	}

	if (yunutyEngine::Input::isKeyDown(KeyCode::Q))
	{
		PlayerController::GetInstance()->SetLeftClickSkill(Unit::SkillEnum::Q);
	}

	if (yunutyEngine::Input::isKeyDown(KeyCode::W))
	{
		PlayerController::GetInstance()->SetLeftClickSkill(Unit::SkillEnum::W);
	}
}
