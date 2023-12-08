#include "PlayerController.h"
#include "RTSCam.h"
#include "Unit.h"

void PlayerController::SetMovingSystemComponent(RTSCam* sys)
{
	m_movingSystemComponent = sys;
}

void PlayerController::Start()
{
	m_movingSystemComponent->groundLeftClickCallback = [this](Vector3d position)
	{
		GetGameObject()->GetComponent<Unit>()->OrderMove(position);
	};

	m_movingSystemComponent->groundRightClickCallback = [this](Vector3d position)
	{
		GetGameObject()->GetComponent<Unit>()->OrderAttackMove(position);
	};
}

