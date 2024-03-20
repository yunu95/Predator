#include "InWanderLand.h"
#include "PlayerController.h"
#include "InputManager.h"
#include "RTSCam.h"
#include "Unit.h"
#include "Dotween.h"
#include "SkillPreviewSystem.h"

void PlayerController::SetMovingSystemComponent(RTSCam* sys)
{
	m_movingSystemComponent = sys;
	m_dotween = sys->GetGameObject()->GetComponent<Dotween>();
}

void PlayerController::AddPlayerUnit(Unit* p_playerUnit)
{
	playerComponentMap.insert({ p_playerUnit->GetPlayerSerialNumber(), p_playerUnit });
}

void PlayerController::SetLeftClickMove()
{
	if (static_cast<int>(currentSelectedSerialNumber) == InputManager::SelectedSerialNumber::All)
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
		Unit* currentSelectedUnit = playerComponentMap.find(currentSelectedSerialNumber)->second;
		m_movingSystemComponent->groundLeftClickCallback = [=](Vector3d pos)
		{
			currentSelectedUnit->OrderMove(pos);
		};
	}

}

void PlayerController::SetLeftClickAttackMove()
{
	if (static_cast<int>(currentSelectedSerialNumber) == InputManager::SelectedSerialNumber::All)
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
	Unit* currentUnit = playerComponentMap.find(currentSelectedSerialNumber)->second;

	if (static_cast<int>(currentSelectedSerialNumber) == InputManager::SelectedSerialNumber::All)
	{
		m_movingSystemComponent->groundLeftClickCallback = [=](Vector3d pos)
		{
			for (auto e : playerComponentMap)
			{
				e.second->OrderSkill(p_skillNum, pos);
			}
		};
	}
	else if (currentUnit->GetCurrentUnitState() != Unit::UnitState::Skill)
	{
		if (static_cast<int>(currentSelectedSerialNumber) == InputManager::SelectedSerialNumber::One && p_skillNum == Unit::SkillEnum::W)
		{
			/// Warrior의 W 스킬은 마우스로 클릭하지 않아도 바로 실행되는 스킬이다. 다른 스킬 나온다면 구조적 개선 필요
			playerComponentMap.find(currentSelectedSerialNumber)->second->OrderSkill(p_skillNum);
		}	

		else
		{
			Unit* currentSelectedUnit = playerComponentMap.find(currentSelectedSerialNumber)->second;
			SkillPreviewSystem::Instance().SetCurrentSelectedPlayerGameObject(currentSelectedUnit->GetGameObject());
			SkillPreviewSystem::Instance().SetCurrentSkillPreviewType(currentSelectedUnit->GetSkillPreviewType(p_skillNum));
			SkillPreviewSystem::Instance().ActivateSkillPreview(true);
			m_movingSystemComponent->groundLeftClickCallback = [=](Vector3d pos)
			{
				SkillPreviewSystem::Instance().ActivateSkillPreview(false);
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

void PlayerController::SetCurrentPlayerSerialNumber(Unit::UnitType p_num)
{
	currentSelectedSerialNumber = p_num;
	Unit* currentSelectedUnit = playerComponentMap.find(currentSelectedSerialNumber)->second;
	Vector3d unitPos = currentSelectedUnit->GetTransform()->GetWorldPosition();
	m_dotween->DOMove(unitPos + cameraOffset, cameraMoveDuration);
	m_dotween->DORotate(Vector3d(60, 0, 0), cameraMoveDuration);
	
	Vector3d camRotation = m_movingSystemComponent->GetTransform()->GetWorldRotation().Euler();
	//m_dotween->DOLookAt(unitPos, cameraMoveDuration, true);
	
	//m_movingSystemComponent->GetTransform()->SetWorldPosition({ unitPos + cameraOffset });

	SetLeftClickMove();
}

std::unordered_map<Unit::UnitType, Unit*> PlayerController::GetPlayerMap() const
{
	return playerComponentMap;
}

Unit* PlayerController::FindSelectedUnitByUnitType(Unit::UnitType p_type)
{
	return playerComponentMap.find(p_type)->second;
}



