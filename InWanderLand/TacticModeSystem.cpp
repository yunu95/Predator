#include "TacticModeSystem.h"
#include "Unit.h"
#include "InputManager.h"
#include "RTSCam.h"
#include "PlayerController.h"
#include "SkillPreviewSystem.h"

TacticModeSystem::TacticModeSystem()
{
    m_queueSelector.insert({ Unit::UnitType::Warrior, &warriorQueue });
    m_queueSelector.insert({ Unit::UnitType::Magician, &magicianQueue });
    m_queueSelector.insert({ Unit::UnitType::Healer, &healerQueue });
}

void TacticModeSystem::SetCurrentSelectedPlayerUnit(Unit::UnitType p_type)
{
    currentSelectedUnit = PlayerController::SingleInstance().FindSelectedUnitByUnitType(p_type);
}

void TacticModeSystem::SetLeftClickAddQueueForMove(InputManager::SelectedSerialNumber currentSelectedNum)
{
	currentSelectedUnit = playerComponentMap.find(static_cast<Unit::UnitType>(currentSelectedNum))->second;
	if (tacticModeGauge > 0)
	{
		m_rtsCam->groundLeftClickCallback = [=](Vector3d pos)
			{
				currentSelectedUnit->PushMoveFunctionToTacticQueue(pos);
				SkillPreviewSystem::Instance().ActivateSkillPreview(false);
				m_rtsCam->groundLeftClickCallback = [](Vector3d pos) {};
				tacticModeGauge--;
			};
	}
}

void TacticModeSystem::SetLeftClickAddQueueForAttackMove(InputManager::SelectedSerialNumber currentSelectedNum)
{
    currentSelectedUnit = playerComponentMap.find(static_cast<Unit::UnitType>(currentSelectedNum))->second;
    if (tacticModeGauge > 0)
    {
		m_rtsCam->groundLeftClickCallback = [=](Vector3d pos)
			{
				currentSelectedUnit->PushAttackMoveFunctionToTacticQueue(pos);
				SkillPreviewSystem::Instance().ActivateSkillPreview(false);
				m_rtsCam->groundLeftClickCallback = [](Vector3d pos) {};
                tacticModeGauge--;
				SetLeftClickAddQueueForMove(currentSelectedNum);
			};
    }
}

void TacticModeSystem::SetLeftClickAddQueueForSkill(InputManager::SelectedSerialNumber currentSelectedNum, Unit::SkillEnum currentSelectedSkill)
{
    /// 이렇게 하지말고 동작할 유닛과 같이 지정해줘서 원하는 유닛이 정확히 동작하도록 해야한다...멍청이
    currentSelectedUnit = playerComponentMap.find(static_cast<Unit::UnitType>(currentSelectedNum))->second;
    processingUnitMap.insert({ queueOrderIndex, currentSelectedUnit });

    SkillPreviewSystem::Instance().SetCurrentSelectedPlayerUnit(currentSelectedUnit);
    SkillPreviewSystem::Instance().SetCurrentSkillPreviewType(currentSelectedUnit->GetSkillPreviewType(currentSelectedSkill));
    SkillPreviewSystem::Instance().SetCurrentSelectedSkillNum(currentSelectedSkill);
    SkillPreviewSystem::Instance().ActivateSkillPreview(true);

    if (tacticModeGauge > 0)
    {
		m_rtsCam->groundLeftClickCallback = [=](Vector3d pos)
			{
				currentSelectedUnit->PushSkillFunctionToTacticQueue(currentSelectedSkill, pos);
				SkillPreviewSystem::Instance().ActivateSkillPreview(false);
				m_rtsCam->groundLeftClickCallback = [](Vector3d pos) {};
                tacticModeGauge--;
				SetLeftClickAddQueueForMove(currentSelectedNum);
            };
    }
 
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
	isTacticModeOperating = true;

    vector<GameObject*> unitGameObjects;
    for (auto each : playerComponentMap)
    {
        unitGameObjects.push_back(each.second->GetGameObject());
    }
    m_rtsCam->SetTargets(unitGameObjects);
    
}

void TacticModeSystem::ExitTacticMode()
{
    Time::SetTimeScale(1.0f);

    isTacticModeOperating = false;

    for (auto each : playerComponentMap)
    {
        if (!each.second->IsTacticModeQueueEmpty())
            each.second->SetUnitStateIdle();
    }

 //   //// 하나라도 Queue에 등록되어 있다면 전술모드를 실행한다.
	//if (!(warriorQueue.empty() && magicianQueue.empty() && healerQueue.empty()))
	//{
	//	isTacticModeStarted = true;
	//}
 //   else
 //   {
 //       isTacticModeStarted = false;
 //   }

 //   if (isTacticModeStarted)
 //   {
	//	if (!(warriorQueue.empty()))
	//	{
	//		CallQueueFunction(playerComponentMap.find(Unit::UnitType::Warrior)->second);
	//	}
	//	if (!(magicianQueue.empty()))
	//	{
	//		CallQueueFunction(playerComponentMap.find(Unit::UnitType::Magician)->second);
	//	}
	//	if (!(healerQueue.empty()))
	//	{
	//		CallQueueFunction(playerComponentMap.find(Unit::UnitType::Healer)->second);
	//	}
 //   }
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
    currentSelectedQueue = m_queueSelector.find(p_currentUnit->GetUnitType())->second;

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
    }
}

