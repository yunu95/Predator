#include "PlaytimeWave.h"
#include "UnitData.h"
#include "WaveData.h"
#include "UnitProductor.h"
#include "Unit.h"
#include "MeleeEnemyProductor.h"
#include "RangedEnemyProductor.h"
#include "Application.h"
#include "ContentsLayer.h"
//#include "UnitObjectPool.h"
#include "MeleeEnemyPool.h"
#include "RangedEnemyPool.h"
#include "ShortcutSystem.h"

#include "RTSCam.h"
#include "TacticModeSystem.h"
#include "InWanderLand.h"

PlaytimeWave::~PlaytimeWave()
{
    waveData->playtimeWave = nullptr;
}
std::weak_ptr<PlaytimeWave> PlaytimeWave::GetCurrentOperatingWave()
{
    return currentOperativeWave;
}
std::weak_ptr<PlaytimeWave> PlaytimeWave::currentOperativeWave;
void PlaytimeWave::ActivateWave()
{
    isWaveActivated = true;
    currentOperativeWave = GetGameObject()->GetComponentWeakPtr<PlaytimeWave>();
    array<int, 3> comboObjectives = { waveData->pod.comboObjective1, waveData->pod.comboObjective2, waveData->pod.comboObjective3 };
    PlayerController::Instance().SetComboObjectives(comboObjectives);
    UIManager::Instance().ShowComboObjectives();
    /// 플레이어 유닛 전투상태 돌입
    PlayerController::Instance().OnWaveStart(GetWeakPtr<PlaytimeWave>());
    //TacticModeSystem::Instance().RegisterCurrentWave(this);

    // 카메라 가동범위 제한
    if (auto rtsCam = dynamic_cast<RTSCam*>(graphics::Camera::GetMainCamera()))
    {
        PlayerController::Instance().LockCamInRegion(waveData->pod.contraintRegion);
        //rtsCam->ConstrainByRegion(waveData->pod.contraintRegion);
    }
}
void PlaytimeWave::DeActivateWave()
{
    waveDataIndex = 0;
    PlayerController::Instance().OnWaveEnd(GetWeakPtr<PlaytimeWave>());
    UIManager::Instance().HideComboObjectvies();
    this->SetActive(false);
    // 카메라 가동범위 제한
    if (auto rtsCam = dynamic_cast<RTSCam*>(graphics::Camera::GetMainCamera()))
    {
        PlayerController::Instance().LockCamInRegion(nullptr);
        //rtsCam->UnConstrainRegion();
    }
}

void PlaytimeWave::Start()
{
}

void PlaytimeWave::Update()
{
    if (isWaveActivated && !isStoppedByTacticMode)
    {
        m_elapsed += Time::GetDeltaTime();
    }
    auto& waveDelays = waveData->pod.waveDelays;
    auto waveSize = waveData->pod.waveSizes[currentSequenceIndex];
    auto sequenceSize = waveData->pod.waveSizes.size();
    // waveDelays는 delay가 오름차순으로 정렬되어 있다.
    // 현재 웨이브에서 아직 소환할 유닛이 더 남았는가?
    if (nextSummonUnitIndex < waveSize)
    {
        // 유닛 소환이 임박했는가?
        while (nextSummonUnitIndex < waveSize && waveDelays[waveDataIndex] < m_elapsed)
        {
            // 유닛을 소환하고 인덱스를 증가시킨다.
            // 유닛 데이터는 아래 값을 사용하면 됨.
            auto unitData = waveData->waveUnitDatasVector[waveDataIndex];
            unitData->inGameUnit = UnitPool::SingleInstance().Borrow(unitData);

            nextSummonUnitIndex++;
            waveDataIndex++;
        }
    }
    // 현재 웨이브에서 소환 대상이 되는 유닛들이 다 소환된 경우
    else
    {
        bool isAllUnitTerminated = true;

        for (auto& e : m_currentWaveUnitVector)
        {
            // 한 유닛이라도 살아 있다면 bool값을 false로
            if (e->IsAlive())
            {
                isAllUnitTerminated = false;
                break;
            }
        }

        // 현재 등장한 유닛들이 다 죽었는가?
        if (isAllUnitTerminated)
        {
            m_currentWaveUnitVector.clear();
            m_elapsed = 0.0f;
            currentSequenceIndex++;
            nextSummonUnitIndex = 0;
            // 웨이브에서 더 진행할 시퀀스가 없다면 웨이브를 중지하라.
            if (currentSequenceIndex >= sequenceSize)
            {
                DeActivateWave();
            }

        }
    }
}

void PlaytimeWave::StopWaveElapsedTime()
{
    isStoppedByTacticMode = true;
}

void PlaytimeWave::ResumeWaveElapsedTime()
{
    isStoppedByTacticMode = false;
}
