#include "TacticModeSystem.h"
#include "Unit.h"
#include "InputManager.h"
#include "RTSCam.h"
#include "PlayerController.h"


void TacticModeSystem::SetCurrentSelectedPlayerUnit(Unit::UnitType p_type)
{
	currentSelectedUnit = PlayerController::GetInstance()->FindSelectedUnitByUnitType(p_type);
}

void TacticModeSystem::SetLeftClickAddQueueForAttackMove(InputManager::SelectedSerialNumber currentSelectedNum)
{
	currentSelectedUnit = playerComponentMap.find(static_cast<Unit::UnitType>(currentSelectedNum))->second;
	SetCurrentSelectedQueue(currentSelectedUnit);
	m_rtsCam->groundLeftClickCallback = [=](Vector3d pos)
	{
		currentSelectedQueue->push([=]()
			{
				currentSelectedUnit->OrderAttackMove(pos);
			});
	};
}

void TacticModeSystem::SetLeftClickAddQueueForSkill(InputManager::SelectedSerialNumber currentSelectedNum, Unit::SkillEnum currentSelectedSkill)
{
	currentSelectedUnit = playerComponentMap.find(static_cast<Unit::UnitType>(currentSelectedNum))->second;
	SetCurrentSelectedQueue(currentSelectedUnit);
	m_rtsCam->groundLeftClickCallback = [=](Vector3d pos)
	{
		currentSelectedQueue->push([=]()
			{
				currentSelectedUnit->OrderSkill(currentSelectedSkill, pos);
			});
	};
}

void TacticModeSystem::EngageTacticMode()
{
	/// 전술모드 키 입력 시 실행될 로직.
	/// 1. TimeScale을 0으로 설정한다.
	/// 2. PlayerController에서 현재 전술모드 적용 가능한 Player Unit의 정보를 가져온다.
	istacticModeOn = true;
	Time::SetTimeScale(0.00001f);
	playerComponentMap = PlayerController::GetInstance()->GetPlayerMap();
}

void TacticModeSystem::ExitTacticMode()
{
	istacticModeOn = false;
	Time::SetTimeScale(1.0f);

	if (!(warriorQueue.empty() && magicianQueue.empty() && healerQueue.empty()))
	{
		isReadyToActivateQueue = true;		/// 이때 유닛은 전술모드에 설정했던 함수를 순차적으로 실행합니다. CallQueueFunction 호출
	}
}

void TacticModeSystem::SetMovingSystemComponent(RTSCam* sys)
{
	m_rtsCam = sys;
}

bool TacticModeSystem::IsTacticModeActivated() const
{
	return isReadyToActivateQueue;
}

bool TacticModeSystem::IsAllSkillActivated() const
{
	return isAllFunctionActivated;
}

void TacticModeSystem::SetCurrentSelectedQueue(Unit* p_currentUnit)
{
	switch (p_currentUnit->GetUnitType())
	{
		case Unit::UnitType::Warrior:
			currentSelectedQueue = &warriorQueue;
			break;
		case Unit::UnitType::Magician:
			currentSelectedQueue = &magicianQueue;
			break;
		case Unit::UnitType::Healer:
			currentSelectedQueue = &healerQueue;
			break;
		default:
			break;
	}
}

void TacticModeSystem::CallQueueFunction(Unit* p_unit)
{
	SetCurrentSelectedQueue(p_unit);

	if (!currentSelectedQueue->empty())
	{
		std::function<void()> tempFunc = currentSelectedQueue->front();
		tempFunc();
		currentSelectedQueue->pop();
	}

	if (currentSelectedQueue->empty())
	{
		isReadyToActivateQueue = false;
		isAllFunctionActivated = true;
	}
}
