#include "TacticModeSystem.h"
#include "Unit.h"
#include "InputManager.h"
#include "RTSCam.h"
#include "PlayerController.h"
#include "SkillPreviewSystem.h"
#include "CursorDetector.h"
#include "PlaytimeWave.h"
#include "WaveData.h"
#include "LocalTimeEntityManager.h"
#include "PlayerSkillSystem.h"
#include "InWanderLand.h"


void TacticModeSystem::OnEnable()
{
    AddGauge(m_maxGauge);
}

void TacticModeSystem::Start()
{
    AddGauge(m_maxGauge);

    isCoolTime = false;
    SetCurrentCoolTimeElapsed(0.0f);

    gaugeRecoveryPerSecond = application::GlobalConstant::GetSingletonInstance().pod.skillGaugeRecoveryPerSecond;
    skillCost = application::GlobalConstant::GetSingletonInstance().pod.tacticSkillCost;
    moveCost = application::GlobalConstant::GetSingletonInstance().pod.tacticMoveCost;
    attackCost = application::GlobalConstant::GetSingletonInstance().pod.tacticAttackCost;
}

void TacticModeSystem::Update()
{
    if (!isTacticModeOperating)
    {
        AddGauge(gaugeRecoveryPerSecond * Time::GetDeltaTime());
    }

    if (isCoolTime)
    {
        SetCurrentCoolTimeElapsed(m_engageCoolTimeElapsed + Time::GetDeltaTime());
        if (m_engageCoolTimeElapsed >= m_engageCoolTimeDuration)
        {
            SetCurrentCoolTimeElapsed(m_engageCoolTimeDuration);
            isCoolTime = false;
        }
    }
}

void TacticModeSystem::PlayFunction()
{
    this->SetActive(true);
    if (isOncePaused)
    {
        Start();
    }
}

void TacticModeSystem::StopFunction()
{
    m_currentWaveUnits.clear();
    playerComponentMap.clear();

    for (int i = 0; i < sequenceQueue.size(); i++)
    {
        sequenceQueue.pop();
    }

    m_maxGauge = 10;
    m_currentGauge = 0;

    isCoolTime = false;
    m_engageCoolTimeDuration = 5.0f;
    m_engageCoolTimeElapsed = 0.0f;

    isTacticModeOperating = false;
    isTacticOrderPerforming = false;
    currentSelectedUnit = nullptr;
    m_currentOperatingWave = nullptr;

    this->SetActive(false);
}

void TacticModeSystem::ToggleRequested(InputManager::SelectedSerialNumber currentSelectedNum)
{
    m_latestSelectedUnitNum = currentSelectedNum;
    if (m_latestSelectedUnitNum == 0)
        m_latestSelectedUnitNum = InputManager::One;

    if (!isTacticOrderPerforming)           /// 1. 명령 실행 중이 아니고,
    {
        if (!isCoolTime)
        {
			if (!isTacticModeOperating)     /// 2. 명령 입력 중이 아니면 진입.
			{
				EngageTacticMode();
				SetTacticModeRightClickFunction(currentSelectedNum);
			}
			else                            /// 3. 명령 입력 중이라면 나가기.
			{
				ExitTacticMode();
                PlayerController::Instance().SetCurrentPlayerSerialNumber(static_cast<Unit::UnitType>(currentSelectedNum));
                PlayerController::Instance().SetRightClickFunction();
			}
        }
    }
}

void TacticModeSystem::SetTacticModeRightClickFunction(InputManager::SelectedSerialNumber currentSelectedNum)
{
    if (currentSelectedNum == 0)
        currentSelectedNum = InputManager::SelectedSerialNumber::One;

    currentSelectedUnit = playerComponentMap.find(static_cast<Unit::UnitType>(currentSelectedNum))->second;
 
        m_rtsCam->groundRightClickCallback = [=](Vector3d pos)
            {
                    if (m_cursorDetector->GetCurrentOnMouseUnit() == nullptr)
                    {
                        if (m_currentGauge - moveCost > 0)
                        {
							currentSelectedUnit->PushMoveFunctionToTacticQueue(pos);
							AddGauge(-1 * moveCost);
                            sequenceQueue.push(currentSelectedUnit);
                        }
                    }             
					else if (Unit* selectedUnit = m_cursorDetector->GetCurrentOnMouseUnit();
						selectedUnit->GetUnitSide() == Unit::UnitSide::Enemy)
					{
                        if (m_currentGauge - attackCost > 0)
                        {
							currentSelectedUnit->PushAttackMoveFunctionToTacticQueue(pos, selectedUnit);
							AddGauge(-1 * attackCost);
                            sequenceQueue.push(currentSelectedUnit);
                        }
					}
                    SkillPreviewSystem::Instance().ActivateSkillPreview(false);
            };
}

void TacticModeSystem::SetLeftClickAddQueueForAttackMove(InputManager::SelectedSerialNumber currentSelectedNum)
{
    currentSelectedUnit = playerComponentMap.find(static_cast<Unit::UnitType>(currentSelectedNum))->second;

        m_rtsCam->groundRightClickCallback = [=](Vector3d pos)
            {
                if (m_currentGauge > 0)
                {
					currentSelectedUnit->PushAttackMoveFunctionToTacticQueue(pos);
					sequenceQueue.push(currentSelectedUnit);
					SkillPreviewSystem::Instance().ActivateSkillPreview(false);
					//m_rtsCam->groundLeftClickCallback = [](Vector3d pos) {};
					//tacticModeGauge--;
					//SetTacticModeRightClickFunction(currentSelectedNum);
					//AddGauge(--m_currentGauge);
                }
            };
}

void TacticModeSystem::SetLeftClickAddQueueForSkill(InputManager::SelectedSerialNumber currentSelectedNum, Unit::SkillEnum currentSelectedSkill)
{
    currentSelectedUnit = playerComponentMap.find(static_cast<Unit::UnitType>(currentSelectedNum))->second;

    SkillPreviewSystem::Instance().SetCurrentSelectedPlayerUnit(currentSelectedUnit);
    SkillPreviewSystem::Instance().SetCurrentSkillPreviewType(currentSelectedUnit->GetSkillPreviewType(currentSelectedSkill));
    SkillPreviewSystem::Instance().SetCurrentSelectedSkillNum(currentSelectedSkill);
    SkillPreviewSystem::Instance().ActivateSkillPreview(true);

    m_rtsCam->groundLeftClickCallback = [=](Vector3d pos)
        {
            if (m_currentGauge - skillCost > 0)
            {
				currentSelectedUnit->PushSkillFunctionToTacticQueue(currentSelectedSkill, pos);
				sequenceQueue.push(currentSelectedUnit);
				SkillPreviewSystem::Instance().ActivateSkillPreview(false);
				SetTacticModeRightClickFunction(currentSelectedNum);
				AddGauge(-1 * skillCost);
            }
        };
}

void TacticModeSystem::RegisterTutorialQuestAction(Unit::UnitType p_targetUnit, OrderType p_orderType)
{

}

void TacticModeSystem::EngageTacticMode()
{
    /// 전술모드 키 입력 시 실행될 로직.
    /// 1. TimeScale을 0으로 설정한다.
    /// 2. PlayerController에서 현재 전술모드 적용 가능한 Player Unit의 정보를 가져온다.
    //Time::SetTimeScale(0.0f);

    for (int i = 0; i < sequenceQueue.size(); i++)
    {
        sequenceQueue.pop();
    }

    m_currentOperatingWave->StopWaveElapsedTime();
    playerComponentMap = PlayerController::Instance().GetPlayerMap();
    isTacticModeOperating = true;

    LocalTimeEntityManager::Instance().ReportTacticModeEngaged();

    /// 적군 유닛들의 현재 애니메이션도 꺼주자.
    for (auto each : m_currentWaveUnits)
    {
        each->EnemyActionOnTacticModeEngaged();
    }

	for (auto each : playerComponentMap)
	{
		each.second->EnemyActionOnTacticModeEngaged();
	}

    vector<GameObject*> unitGameObjects;
    for (auto each : playerComponentMap)
    {
        unitGameObjects.push_back(each.second->GetGameObject());
    }
    m_rtsCam->SetTargets(unitGameObjects);

}

void TacticModeSystem::ExitTacticMode()
{
    //Time::SetTimeScale(1.0f);    

    /// 플레이어 및 적군의 시간을 Resume합니다. 
    /// 이 때 플레이어는 입력된 명령을 실제로 실행하고, 적군 유닛은 시간이 멈춘 듯이 설정해줍니다.
    for (auto each : playerComponentMap)
    {
        LocalTimeEntityManager::Instance().SetLocalTimeScaleDirectly(each.second, 1.0f);
        LocalTimeEntityManager::Instance().SetLocalTimeScaleDirectly(each.second->GetGameObject()->GetComponent<Dotween>(), 1.0f);
        each.second->GetGameObject()->GetComponent<PlayerSkillSystem>()->SetSkillRequirmentLocalTimeScale(1.0f);
    }

    isTacticModeOperating = false;

    for (auto each : playerComponentMap)
    {
        if (!each.second->IsTacticModeQueueEmpty())
            each.second->SetUnitStateIdle();
    }

    if (sequenceQueue.empty())
    {
		SetCurrentCoolTimeElapsed(0.0f);
		isCoolTime = true;
		isTacticOrderPerforming = false;
		LocalTimeEntityManager::Instance().ReportTacticModeEnded();
		m_currentOperatingWave->ResumeWaveElapsedTime();

		for (auto each : m_currentWaveUnits)
		{
			each->EnemyActionOnTacticModeEnded();
		}
		for (auto each : playerComponentMap)
		{
			each.second->EnemyActionOnTacticModeEnded();
		}
		m_rtsCam->SetTarget(PlayerController::Instance().GetPlayerMap().find(static_cast<Unit::UnitType>(m_latestSelectedUnitNum))->second->GetGameObject());
    }
    else
    {
		isTacticOrderPerforming = true;
		m_rtsCam->SetTarget(sequenceQueue.front()->GetGameObject());
        sequenceQueue.front()->EnemyActionOnTacticModeEnded();
		sequenceQueue.front()->PermitTacticAction();
		sequenceQueue.pop();
    }
}

void TacticModeSystem::SetMovingSystemComponent(RTSCam* sys)
{
    m_rtsCam = sys;
}

bool TacticModeSystem::IsOrderingTimingNow() const
{
    return isTacticModeOperating;
}

bool TacticModeSystem::IsUnitsPerformingCommand() const
{
    return isTacticOrderPerforming;
}

bool TacticModeSystem::IsTacticModeCoolTime() const
{
    return isCoolTime;
}

void TacticModeSystem::SetCurrentCoolTimeElapsed(float elapsed)
{
    m_engageCoolTimeElapsed = elapsed;
    if (isCoolTime)
    {
        UIManager::Instance().GetUIElementByEnum(UIEnumID::Toggle_TacticMode_Overlay)->adjuster->SetTargetFloat(elapsed / m_engageCoolTimeDuration);
        UIManager::Instance().GetUIElementByEnum(UIEnumID::Toggle_TacticMode_Cooltime_Number)->SetNumber(m_engageCoolTimeDuration - elapsed);
    }
    else
    {
        UIManager::Instance().GetUIElementByEnum(UIEnumID::Toggle_TacticMode_Overlay)->adjuster->SetTargetFloat(1);
        UIManager::Instance().GetUIElementByEnum(UIEnumID::Toggle_TacticMode_Cooltime_Number)->DisableElement();
    }
}

float TacticModeSystem::GetLeftCoolTime()
{
    if (m_engageCoolTimeElapsed >= m_engageCoolTimeDuration)
        return 0.0f;
    else
        return m_engageCoolTimeDuration - m_engageCoolTimeElapsed;
}

void TacticModeSystem::AddGauge(int p_gauge)
{
    m_currentGauge += p_gauge;

    if (m_currentGauge < 0)
    {
        m_currentGauge = 0;
    }
    else if (m_currentGauge > m_maxGauge)
    {
        m_currentGauge = m_maxGauge;
    }
}

void TacticModeSystem::ReportTacticActionFinished()
{
    /// 전술모드의 수행을 완료한 유닛이 해당 함수를 호출합니다.
    if (sequenceQueue.empty())
    {
		SetCurrentCoolTimeElapsed(0.0f);
		isCoolTime = true;
        isTacticOrderPerforming = false;
        LocalTimeEntityManager::Instance().ReportTacticModeEnded();
		m_currentOperatingWave->ResumeWaveElapsedTime();
        
        for (auto each : m_currentWaveUnits)
        {
            each->EnemyActionOnTacticModeEnded();
        }
		for (auto each : playerComponentMap)
		{
			each.second->EnemyActionOnTacticModeEnded();
		}
    }
    else
    {
        m_rtsCam->SetTarget(sequenceQueue.front()->GetGameObject());
        sequenceQueue.front()->PermitTacticAction();
        sequenceQueue.pop();
    }
}

void TacticModeSystem::RegisterCurrentWave(PlaytimeWave* p_wave)
{
    m_currentOperatingWave = p_wave;
    for (auto each : p_wave->waveData->waveUnitDatasVector)
    {
        m_currentWaveUnits.push_back(each->inGameUnit);
    }
}

