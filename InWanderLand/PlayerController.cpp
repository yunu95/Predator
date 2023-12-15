#include "PlayerController.h"
#include "RTSCam.h"
#include "Unit.h"

PlayerController* PlayerController::instance = nullptr;

PlayerController::PlayerController()
{
	instance = this;
}

PlayerController::~PlayerController()
{
	delete instance;
}

PlayerController* PlayerController::GetInstance()
{
	if (instance == nullptr)
		instance = new PlayerController;
	return instance;
}

void PlayerController::SetMovingSystemComponent(RTSCam* sys)
{
	m_movingSystemComponent = sys;
}

void PlayerController::AddPlayerUnit(Unit* p_playerUnit)
{
	playerComponentMap.insert({p_playerUnit, p_playerUnit->GetPlayerSerialNumber()});
}

void PlayerController::MakeLeftClickMove(int unitSerialNumber)
{
	for (auto e : playerComponentMap)
	{
		if (e.second == unitSerialNumber)
		{
			m_movingSystemComponent->GetMouseCursorObject()
				->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>()->GetGI().GetMaterial()->SetColor(yunuGI::Color{ 0, 1, 1, 0 });
			m_movingSystemComponent->groundLeftClickCallback = [=](Vector3d position) { e.first->OrderMove(position); };
			break;
		}
	}
}

void PlayerController::MakeLeftClickAttackMove(int unitSerialNumber)
{
	for (auto e : playerComponentMap)
	{
		if (e.second == unitSerialNumber)
		{
			m_movingSystemComponent->GetMouseCursorObject()
				->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>()->GetGI().GetMaterial()->SetColor(yunuGI::Color{ 0, 0, 1, 0 });
			m_movingSystemComponent->groundLeftClickCallback = [=](Vector3d position) { e.first->OrderAttackMove(position); };
			break;
		}
	}
}

void PlayerController::QSkillKeyPressed(int unitSerialNumber)
{
	m_movingSystemComponent->GetMouseCursorObject()
		->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>()->GetGI().GetMaterial()->SetColor(yunuGI::Color{ 1, 1, 1, 0 });
	for (auto e : playerComponentMap)
	{
		if (e.second == unitSerialNumber)
		{

			m_movingSystemComponent->groundLeftClickCallback = [=](Vector3d position)
			{
				// 마우스 커서에 스킬 범위 출력 (임시)
				m_movingSystemComponent->GetMouseCursorObject()
					->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>()->GetGI().GetMaterial()->SetColor(yunuGI::Color{ 1, 0, 0, 0 });
				e.first->OrderQSkill(position); 
			};
			break;
		}
	}
	
}

