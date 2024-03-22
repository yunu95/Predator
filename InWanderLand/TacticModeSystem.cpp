#include "TacticModeSystem.h"
#include "Unit.h"
#include "InputManager.h"
#include "RTSCam.h"
#include "PlayerController.h"

TacticModeSystem::TacticModeSystem()
{
	m_queueSelector.insert({ "SKM_Robin" , &warriorQueue });
}

void TacticModeSystem::SetCurrentSelectedPlayerUnit(Unit::UnitType p_type)
{
	currentSelectedUnit = PlayerController::SingleInstance().FindSelectedUnitByUnitType(p_type);
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
	/// 이렇게 하지말고 동작할 유닛과 같이 지정해줘서 원하는 유닛이 정확히 동작하도록 해야한다...멍청이
	currentSelectedUnit = playerComponentMap.find(static_cast<Unit::UnitType>(currentSelectedNum))->second;
	processingUnitMap.insert({ queueOrderIndex, currentSelectedUnit });

	SkillPreviewSystem::Instance().SetCurrentSelectedPlayerGameObject(currentSelectedUnit->GetGameObject());
	SkillPreviewSystem::Instance().SetCurrentSkillPreviewType(currentSelectedUnit->GetSkillPreviewType(currentSelectedSkill));
	SkillPreviewSystem::Instance().ActivateSkillPreview(true);

	m_rtsCam->groundLeftClickCallback = [=](Vector3d pos)
	{
		processingSkillPosMap.insert({ currentSelectedUnit, pos });
		queueOrderIndex++;

		SetCurrentSelectedQueue(currentSelectedUnit);

		SkillPreviewSystem::Instance().ActivateSkillPreview(false);

		currentSelectedQueue->push([=]()
			{
				if (auto itr = processingSkillPosMap.find(currentActivatedUnit);
					itr != processingSkillPosMap.end())
				{
					Vector3d currentSkillPos = processingSkillPosMap.find(currentActivatedUnit)->second;
					currentActivatedUnit->OrderSkill(currentSelectedSkill, currentSkillPos);
					processingSkillPosMap.erase(itr);
					executingOrderIndex++;
				}
			});
	};
}

void TacticModeSystem::EngageTacticMode()
{
	/// 전술모드 키 입력 시 실행될 로직.
	/// 1. TimeScale을 0으로 설정한다.
	/// 2. PlayerController에서 현재 전술모드 적용 가능한 Player Unit의 정보를 가져온다.
	Time::SetTimeScale(0.0f);
	playerComponentMap = PlayerController::SingleInstance().GetPlayerMap();
	queueOrderIndex = 0;
	executingOrderIndex = 0;
	processingUnitMap.clear();
	processingSkillPosMap.clear();
}

void TacticModeSystem::ExitTacticMode()
{
	Time::SetTimeScale(1.0f);

	// 하나라도 Queue에 등록되어 있다면 전술모드를 실행한다.
	if (!(warriorQueue.empty() && magicianQueue.empty() && healerQueue.empty()))
	{
		isTacticModeStarted = true;
	}
}

void TacticModeSystem::SetMovingSystemComponent(RTSCam* sys)
{
	m_rtsCam = sys;
}

bool TacticModeSystem::IsTacticModeActivated(Unit* p_unit)
{
	if (p_unit->GetUnitSide() != Unit::UnitSide::Player)
		return false;

	SetCurrentSelectedQueue(p_unit);

	if (currentSelectedQueue->empty())
		return false;
	else
		return true;
}

void TacticModeSystem::SetCurrentSelectedQueue(Unit* p_currentUnit)
{
	std::string unitFbxName = p_currentUnit->GetUnitFbxName();

	currentSelectedQueue = m_queueSelector.find(unitFbxName)->second;

	currentActivatedUnit = p_currentUnit;
}

void TacticModeSystem::CallQueueFunction(Unit* p_unit)
{
	SetCurrentSelectedQueue(p_unit);

	if (!currentSelectedQueue->empty())
	{
		std::function<void()> tempFunc = currentSelectedQueue->front();
		//currentSelectedQueue->front()();
		tempFunc();
		currentSelectedQueue->pop();

		if (warriorQueue.empty() && magicianQueue.empty() && healerQueue.empty())
			isTacticModeStarted = false;
	}
}

