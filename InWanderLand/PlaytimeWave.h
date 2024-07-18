#pragma once
#include "YunutyEngine.h"
#include "Unit.h"
#include <string>
#include <map>
#include "ContentsObservee.h"
#include "StaticInstanceRegistry.h"
#include "ProgressTracker.h"

namespace application
{
    namespace editor
    {
        class POD_Unit;
        class WaveData;
    }
}

class UnitProductor;
class PlayerController;

/// <summary>
/// 플레이타임에서 웨이브 하나에 대응되는 컴포넌트
/// </summary>
class PlaytimeWave : public Component, public ContentsObservee, public StaticInstanceRegistry<PlaytimeWave>, public application::ProgressTracker
{
private:
    int currentSequenceIndex{ 0 };
    int nextSummonUnitIndex{ 0 };
    int waveDataIndex{ 0 };

    bool isWaveActivated = false;
    bool isWaveFinished = false;

    float m_elapsed{ 0.0f };

    bool isStoppedByTacticMode{ false };
public:
    static std::weak_ptr<PlaytimeWave> GetCurrentOperatingWave();
    static std::weak_ptr<PlaytimeWave> currentOperativeWave;
    std::set<Unit*> m_currentWaveUnits;
    Unit* inGameUnit;
    virtual ~PlaytimeWave();

    bool IsRemainEnemyAndWave();
    void Reset();

    void ActivateWave();
    void DeActivateWave();

    void ReportUnitDeath(Unit* unit);

    coroutine::Coroutine WaveEndCoroutine(Unit* lastStandingUnit);

    virtual void Start() override;
    virtual void Update() override;
    application::editor::WaveData* waveData{ nullptr };

    void StopWaveElapsedTime();
    void ResumeWaveElapsedTime();
    virtual Component* GetComponent() override { return this; }

    std::unordered_map<int, std::vector<std::function<void()>>> waveStartCallbackMap = std::unordered_map<int, std::vector<std::function<void()>>>();
    std::unordered_map<int, std::vector<std::function<void()>>> waveEndCallbackMap = std::unordered_map<int, std::vector<std::function<void()>>>();
    friend PlayerController;
};