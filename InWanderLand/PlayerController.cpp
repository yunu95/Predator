#include "InWanderLand.h"
#include "PlayerController.h"
#include "InputManager.h"
#include "RTSCam.h"
#include "Unit.h"
#include "Dotween.h"

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
	playerComponentMap.insert({ p_playerUnit->GetPlayerSerialNumber(), p_playerUnit });
}

void PlayerController::SetLeftClickMove()
{
	if (currentSelectedSerialNumber == InputManager::SelectedSerialNumber::All)
	{
		m_movingSystemComponent->groundLeftClickCallback = [=](Vector3d pos)
		{
			for (auto e : playerComponentMap)
			{
				e.second->OrderMove(pos);
			}
		};
	}
	else
	{
		m_movingSystemComponent->groundLeftClickCallback = [=](Vector3d pos)
		{
			playerComponentMap.find(currentSelectedSerialNumber)->second->OrderMove(pos);
		};
	}

}

void PlayerController::SetLeftClickAttackMove()
{
	if (currentSelectedSerialNumber == InputManager::SelectedSerialNumber::All)
	{
		m_movingSystemComponent->groundLeftClickCallback = [=](Vector3d pos)
		{
			for (auto e : playerComponentMap)
			{
				e.second->OrderAttackMove(pos);
			}
		};
	}
	else
	{
		m_movingSystemComponent->groundLeftClickCallback = [=](Vector3d pos)
		{
			playerComponentMap.find(currentSelectedSerialNumber)->second->OrderAttackMove(pos);
		};
	}
}

void PlayerController::SetLeftClickSkill(Unit::SkillEnum p_skillNum)
{
	if (currentSelectedSerialNumber == InputManager::SelectedSerialNumber::All)
	{
		m_movingSystemComponent->groundLeftClickCallback = [=](Vector3d pos)
		{
			for (auto e : playerComponentMap)
			{
				e.second->OrderSkill(p_skillNum, pos);
			}
		};
	}
	else
	{
		if (currentSelectedSerialNumber == InputManager::SelectedSerialNumber::One && p_skillNum == Unit::SkillEnum::W)
		{
			/// Warrior의 W 스킬은 마우스로 클릭하지 않아도 바로 실행되는 스킬이다. 다른 스킬 나온다면 구조적 개선 필요
			playerComponentMap.find(currentSelectedSerialNumber)->second->OrderSkill(p_skillNum);
		}

		else
		{
			m_movingSystemComponent->groundLeftClickCallback = [=](Vector3d pos)
			{
				playerComponentMap.find(currentSelectedSerialNumber)->second->OrderSkill(p_skillNum, pos);
			};
		}
	}
}

void PlayerController::SetLeftClickEmpty()
{
	m_movingSystemComponent->groundLeftClickCallback = [](Vector3d pos) {};
}


void PlayerController::SetRightClickEmpty()
{
	m_movingSystemComponent->groundRightClickCallback = [](Vector3d pos) {};
}

void PlayerController::SetCurrentPlayerSerialNumber(int p_num)
{
	currentSelectedSerialNumber = p_num;
	SetLeftClickMove();
}

//void PlayerController::ApplyCurrentPlayerOrder(int unitSerialNumber, OrderType orderType)
//{
//	m_movingSystemComponent->GetMouseCursorObject()
//		->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>()->GetGI().GetMaterial()->SetColor(yunuGI::Color{ 0, 1, 1, 0 });
//
//	// 이전에 선택한 유닛과 같다면 그대로 둔다.
//	SelectFunctionByOrderType(unitSerialNumber, orderType);
//
//	previousSerialNumber = unitSerialNumber;
//}
//
//void PlayerController::SelectFunctionByOrderType(int unitSerialNumber, OrderType p_orderType)
//{
//	switch (p_orderType)
//	{
//		case PlayerController::OrderType::Move:
//		{
//
//				if (unitSerialNumber == InputManager::SelectedSerialNumber::All)
//				{
//					m_movingSystemComponent->groundRightClickCallback = [=](Vector3d position)
//					{
//						for (auto e : playerComponentMap)
//						{
//							e.first->OrderMove(position);
//						}
//					};
//				}
//				else
//				{
//					for (auto e : playerComponentMap)
//					{
//						if (e.second == unitSerialNumber)
//						{
//							m_movingSystemComponent->groundRightClickCallback = [=](Vector3d position)
//							{
//								e.first->OrderMove(position);
//							};
//						}
//					}
//				}
//			break;
//		}
//
//		case PlayerController::OrderType::AttackMove:
//		{
//			if (unitSerialNumber == InputManager::SelectedSerialNumber::All)
//			{
//				m_movingSystemComponent->groundRightClickCallback = [=](Vector3d position)
//				{
//					for (auto e : playerComponentMap)
//					{
//						e.first->OrderAttackMove(position, true);
//					}
//				};
//			}
//			else
//			{
//				for (auto e : playerComponentMap)
//				{
//					if (e.second == unitSerialNumber)
//					{
//						m_movingSystemComponent->groundRightClickCallback = [=](Vector3d position)
//						{
//							e.first->OrderAttackMove(position, false);
//						};
//					}
//				}
//			}
//			break;
//		}
//		case PlayerController::OrderType::QSkill:
//		{
//			if (unitSerialNumber == InputManager::SelectedSerialNumber::All)
//			{
//				m_movingSystemComponent->groundLeftClickCallback = [=](Vector3d position)
//				{
//					for (auto e : playerComponentMap)
//					{
//						e.first->OrderQSkill(position, true);
//					}
//				};
//			}
//			else
//			{
//				for (auto e : playerComponentMap)
//				{
//					if (e.second == unitSerialNumber)
//					{
//						m_movingSystemComponent->groundLeftClickCallback = [=](Vector3d position)
//						{
//							e.first->OrderQSkill(position, false);
//						};
//					}
//				}
//			}
//			break;
//		}
//	}
//}


