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
    if (waveData->playtimeWave == this)
    {
        waveData->playtimeWave = nullptr;
    }
}
bool PlaytimeWave::IsRemainEnemyAndWave()
{
    return !m_currentWaveUnits.empty() && (currentSequenceIndex < waveData->pod.waveSizes.size());
}
void PlaytimeWave::Reset()
{
    currentSequenceIndex = 0;
    nextSummonUnitIndex = 0;
    waveDataIndex = 0;
    m_elapsed = 0.0f;
    isStoppedByTacticMode = false;
    isWaveActivated = false;
    isWaveFinished = false;
    m_currentWaveUnits.clear();
    SetActive(true);
    if (currentOperativeWave.lock().get() == this)
    {
        PlayerController::Instance().UnlockCamFromRegion();
        currentOperativeWave.reset();
    }
}
std::weak_ptr<PlaytimeWave> PlaytimeWave::GetCurrentOperatingWave()
{
    return currentOperativeWave;
}
std::weak_ptr<PlaytimeWave> PlaytimeWave::currentOperativeWave;
void PlaytimeWave::ActivateWave()
{
    if (isWaveFinished || isWaveActivated)
        return;
    isWaveActivated = true;
    currentOperativeWave = GetGameObject()->GetComponentWeakPtr<PlaytimeWave>();
    m_currentWaveUnits.clear();
    array<int, 3> comboObjectives = { waveData->pod.comboObjective1, waveData->pod.comboObjective2, waveData->pod.comboObjective3 };
    PlayerController::Instance().SetComboObjectives(comboObjectives);
    PlayerController::Instance().triggeredWaves.insert(this);
    UIManager::Instance().ShowComboObjectives();
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
    /// 웨이브 끝날 때, 호출되는 콜백
    if (waveEndCallbackMap.contains(0))
    {
        for (auto& each : waveEndCallbackMap[0])
        {
            each();
        }
    }

    currentOperativeWave.reset();
    waveDataIndex = 0;
    isWaveFinished = true;
    isWaveActivated = false;
    UIManager::Instance().HideComboObjectvies();
    this->SetActive(false);
    PlayerController::Instance().finishedWaves.insert(this);
    // 카메라 가동범위 제한
    if (auto rtsCam = dynamic_cast<RTSCam*>(graphics::Camera::GetMainCamera()))
    {
        PlayerController::Instance().UnlockCamFromRegion();
        //rtsCam->UnConstrainRegion();
    }
}

void PlaytimeWave::ReportUnitDeath(Unit* unit)
{
    if (unit->IsPlayerUnit() || m_currentWaveUnits.empty())
        return;

    if (nextSummonUnitIndex >= waveData->pod.waveSizes[currentSequenceIndex] && m_currentWaveUnits.size() == 1 && currentSequenceIndex + 1 >= waveData->pod.waveSizes.size())
    {
        /// 웨이브 종료 코루틴 실행.
        bool isWaveEndActDone = false;
        auto waveEndCoroutine = ContentsCoroutine::Instance().StartCoroutine(WaveEndCoroutine(unit));
        waveEndCoroutine.lock()->PushDestroyCallBack([=]()
            {
                if (!m_currentWaveUnits.empty())        /// 타이밍이 안좋으면 두 번 불릴 수 있다. 조치는 되었을테지만 혹시 모르니...
                {
					m_currentWaveUnits.erase(m_currentWaveUnits.find(unit));
                }
                else
                {
                    /// 들어와선 안되는 부분. 디버깅 용
                    int a = 0;
                }
            });
    }
    else
    {
		if (m_currentWaveUnits.find(unit) != m_currentWaveUnits.end())
        {
			m_currentWaveUnits.erase(m_currentWaveUnits.find(unit));
		}
        else
        {
			/// 들어와선 안되는 부분. 디버깅 용
            int a = 0;
        }
    }
}

coroutine::Coroutine PlaytimeWave::WaveEndCoroutine(Unit* lastStandingUnit)
{
    auto gc = GlobalConstant::GetSingletonInstance().pod;
    auto beforeZoomFactor = PlayerController::Instance().GetZoomFactor();

    float a = Time::GetTimeScale();
    float b = lastStandingUnit->GetUnitTemplateData().pod.waveEndDestTimeScale;
    float dur = lastStandingUnit->GetUnitTemplateData().pod.waveEndSlowLerpTime;
    float waveEndZoomFactor = lastStandingUnit->GetUnitTemplateData().pod.waveEndZoomFactor;
    float waveEndCameraMoveDuration = lastStandingUnit->GetUnitTemplateData().pod.waveEndCameraMoveDuration;
    float waveEndActionTime = lastStandingUnit->GetUnitTemplateData().pod.waveEndActionTime;

    // 선형 그래프
    //float realElapsedTime = dur * 2 / (b + a);
    float realElapsedTime = dur * 2 / (b + a);

    if (dur <= 0)
        dur = 1;

    //PlayerController::Instance().SetZoomFactor(beforeZoomFactor * gc.waveEndZoomFactor);

    RTSCam::Instance().SetUpdateability(false);
    auto camPivotPoint = lastStandingUnit->GetTransform()->GetWorldPosition();
    Vector3d camStartPos = RTSCam::Instance().GetTransform()->GetWorldPosition();
    Vector3d targetPos = camPivotPoint + PlayerController::Instance().GetCamOffsetNorm() * waveEndZoomFactor;

    coroutine::ForSeconds forSeconds{ waveEndActionTime };
    forSeconds.isRealTime = true;

    auto cameraMoveDuration = waveEndCameraMoveDuration;
    if (cameraMoveDuration > waveEndActionTime)
    {
        cameraMoveDuration = waveEndActionTime;
    }

    while (forSeconds.Tick())
    {
        /// 카메라 무브
        if (forSeconds.Elapsed() <= cameraMoveDuration)
        {
            auto factor = forSeconds.Elapsed() / cameraMoveDuration;
            RTSCam::Instance().GetTransform()->SetWorldPosition(Vector3d::Lerp(camStartPos, targetPos, UICurveFunctions[(int)UICurveType::EaseOutQuad](factor)));
        }

        if (forSeconds.Elapsed() < realElapsedTime)
        {
            // 선형 그래프
            Time::SetTimeScale((b - a) * (a + b) / (2 * dur) * forSeconds.Elapsed() + a);
        }
        else
        {
            Time::SetTimeScale(b);
        }
        co_await std::suspend_always{};

    }
    coroutine::ForSeconds forSecondsAfter{ gc.waveEndRecoveryTime ,true };

    RTSCam::Instance().SetUpdateability(true);
    Time::SetTimeScale(1);
    while (forSecondsAfter.Tick())
    {
        co_await std::suspend_always{};
    }
    //PlayerController::Instance().SetZoomFactor(beforeZoomFactor);

    co_return;
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
            if (m_currentWaveUnits.size() == 0)
            {
                /// Idx 0 는 시작 콜백
                if (currentSequenceIndex == 0)
                {
                    for (auto& each : waveStartCallbackMap[0])
                    {
                        each();
                    }
                }

                /// Idx 맞는 Callback 호출
                if (waveStartCallbackMap.contains(currentSequenceIndex + 1))
                {
                    for (auto& each : waveStartCallbackMap[currentSequenceIndex + 1])
                    {
                        each();
                    }
                }
            }

            // 유닛을 소환하고 인덱스를 증가시킨다.
            // 유닛 데이터는 아래 값을 사용하면 됨.
            auto unitData = waveData->waveUnitDatasVector[waveDataIndex];
            unitData->inGameUnit = UnitPool::SingleInstance().Borrow(unitData);
            unitData->inGameUnit.lock().get()->belongingWave = this;

            m_currentWaveUnits.insert(waveData->waveUnitDatasVector[waveDataIndex]->inGameUnit.lock().get());

            nextSummonUnitIndex++;
            waveDataIndex++;
        }
    }
    // 현재 웨이브에서 소환 대상이 되는 유닛들이 다 소환된 경우
    else
    {
        //for (auto& e : m_currentWaveUnits)
        //{
        //    // 한 유닛이라도 살아 있다면 bool값을 false로
        //    if (e->IsAlive() && e->GetGameObject()->GetActive())
        //    {
        //        break;
        //    }
        //}

        // 현재 등장한 유닛들이 다 죽었는가?
        if (m_currentWaveUnits.size() == 0)
        {
            /// Idx 맞는 Callback 호출
            if (waveEndCallbackMap.contains(currentSequenceIndex + 1))
            {
                for (auto& each : waveEndCallbackMap[currentSequenceIndex + 1])
                {
                    each();
                }
            }

            m_currentWaveUnits.clear();
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
