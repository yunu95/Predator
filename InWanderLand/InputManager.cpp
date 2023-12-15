#include "InputManager.h"
#include "PlayerController.h"

void InputManager::Start()
{
	currentSelectedSerialNumber = 0;
}

void InputManager::Update()
{
	if (yunutyEngine::Input::isKeyDown(KeyCode::NUM_1))
	{
		currentSelectedSerialNumber = 1;
		PlayerController::GetInstance()->MakeLeftClickMove(currentSelectedSerialNumber);
	}
	if (yunutyEngine::Input::isKeyDown(KeyCode::NUM_2))
	{
		currentSelectedSerialNumber = 2;
		PlayerController::GetInstance()->MakeLeftClickMove(currentSelectedSerialNumber);
	}
	if (yunutyEngine::Input::isKeyDown(KeyCode::NUM_3))
	{
		currentSelectedSerialNumber = 3;
		PlayerController::GetInstance()->MakeLeftClickMove(currentSelectedSerialNumber);
	}

	if (yunutyEngine::Input::isKeyDown(KeyCode::A))
	{
		PlayerController::GetInstance()->MakeLeftClickAttackMove(currentSelectedSerialNumber);
	}

	if (yunutyEngine::Input::isKeyDown(KeyCode::Q))
	{
		PlayerController::GetInstance()->QSkillKeyPressed(currentSelectedSerialNumber);
	}
}
