#include "InputManager.h"
#include "PlayerController.h"

void InputManager::Start()
{

}

void InputManager::Update()
{
	if (yunutyEngine::Input::isKeyDown(KeyCode::NUM_1))
	{
		currentSelectedSerialNumber = SelectedSerialNumber::One;
		PlayerController::GetInstance()->ApplyCurrentPlayerOrder(currentSelectedSerialNumber, PlayerController::OrderType::Move);
	}
	if (yunutyEngine::Input::isKeyDown(KeyCode::NUM_2))
	{
		currentSelectedSerialNumber = SelectedSerialNumber::Two;
		PlayerController::GetInstance()->ApplyCurrentPlayerOrder(currentSelectedSerialNumber, PlayerController::OrderType::Move);
	}
	if (yunutyEngine::Input::isKeyDown(KeyCode::NUM_3))
	{
		currentSelectedSerialNumber = SelectedSerialNumber::Three;
		PlayerController::GetInstance()->ApplyCurrentPlayerOrder(currentSelectedSerialNumber, PlayerController::OrderType::Move);
	}

	if (yunutyEngine::Input::isKeyDown(KeyCode::NUM_4))
	{
		currentSelectedSerialNumber = SelectedSerialNumber::All;
		PlayerController::GetInstance()->ApplyCurrentPlayerOrder(currentSelectedSerialNumber, PlayerController::OrderType::Move);
	}

	if (yunutyEngine::Input::isKeyDown(KeyCode::A))
	{
		PlayerController::GetInstance()->ApplyCurrentPlayerOrder(currentSelectedSerialNumber, PlayerController::OrderType::AttackMove);
	}

	if (yunutyEngine::Input::isKeyDown(KeyCode::Q))
	{
		PlayerController::GetInstance()->ApplyCurrentPlayerOrder(currentSelectedSerialNumber, PlayerController::OrderType::QSkill);
	}
}
