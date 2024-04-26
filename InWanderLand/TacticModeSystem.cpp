#include "TacticModeSystem.h"
#include "Unit.h"
#include "InputManager.h"
#include "RTSCam.h"
#include "PlayerController.h"
#include "SkillPreviewSystem.h"
#include "CursorDetector.h"

TacticModeSystem::TacticModeSystem()
{
}

void TacticModeSystem::SetTacticModeRightClickFunction(InputManager::SelectedSerialNumber currentSelectedNum)
{
    if (currentSelectedNum == 0)
        currentSelectedNum = InputManager::One;

	currentSelectedUnit = playerComponentMap.find(static_cast<Unit::UnitType>(currentSelectedNum))->second;
	if (tacticModeGauge > 0)
	{
		m_rtsCam->groundRightClickCallback = [=](Vector3d pos)
			{
				if (m_cursorDetector->GetCurrentOnMouseUnit() == nullptr)
				{
					currentSelectedUnit->PushMoveFunctionToTacticQueue(pos);
				}
				else if (Unit* selectedUnit = m_cursorDetector->GetCurrentOnMouseUnit();
                    selectedUnit->GetUnitSide() == Unit::UnitSide::Enemy)
				{
					currentSelectedUnit->PushAttackMoveFunctionToTacticQueue(pos, selectedUnit);
				}
				SkillPreviewSystem::Instance().ActivateSkillPreview(false);
				//m_rtsCam->groundLeftClickCallback = [](Vector3d pos) {};
				//tacticModeGauge--;
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
				//m_rtsCam->groundLeftClickCallback = [](Vector3d pos) {};
                //tacticModeGauge--;
				SetTacticModeRightClickFunction(currentSelectedNum);
			};
    }
}

void TacticModeSystem::SetLeftClickAddQueueForSkill(InputManager::SelectedSerialNumber currentSelectedNum, Unit::SkillEnum currentSelectedSkill)
{
    currentSelectedUnit = playerComponentMap.find(static_cast<Unit::UnitType>(currentSelectedNum))->second;

    if (tacticModeGauge > 0)
    {
        SkillPreviewSystem::Instance().SetCurrentSelectedPlayerUnit(currentSelectedUnit);
		SkillPreviewSystem::Instance().SetCurrentSkillPreviewType(currentSelectedUnit->GetSkillPreviewType(currentSelectedSkill));
		SkillPreviewSystem::Instance().SetCurrentSelectedSkillNum(currentSelectedSkill);
		SkillPreviewSystem::Instance().ActivateSkillPreview(true);

		m_rtsCam->groundLeftClickCallback = [=](Vector3d pos)
			{
				currentSelectedUnit->PushSkillFunctionToTacticQueue(currentSelectedSkill, pos);
				SkillPreviewSystem::Instance().ActivateSkillPreview(false);
				//m_rtsCam->groundLeftClickCallback = [](Vector3d pos) {};
                //tacticModeGauge--;
				SetTacticModeRightClickFunction(currentSelectedNum);
            };
    }
}

void TacticModeSystem::RegisterTutorialQuestAction(Unit::UnitType p_targetUnit, OrderType p_orderType)
{

}

void TacticModeSystem::EngageTacticMode()
{
    /// 전술모드 키 입력 시 실행될 로직.
    /// 1. TimeScale을 0으로 설정한다.
    /// 2. PlayerController에서 현재 전술모드 적용 가능한 Player Unit의 정보를 가져온다.
    Time::SetTimeScale(0.0f);
    playerComponentMap = PlayerController::SingleInstance().GetPlayerMap();
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

