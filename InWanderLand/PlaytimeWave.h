#pragma once
#include "YunutyEngine.h"
#include "Unit.h"
#include <string>
#include <map>
#include "ContentsObservee.h"
#include "StaticInstanceRegistry.h"

namespace application
{
    namespace editor
    {
        class POD_Unit;
        class WaveData;
    }
}

class UnitProductor;

/// <summary>
/// 플레이타임에서 웨이브 하나에 대응되는 컴포넌트
/// </summary>
class PlaytimeWave : public Component, public ContentsObservee, public StaticInstanceRegistry<PlaytimeWave>
{
private:
    int currentSequenceIndex{ 0 };
    int nextSummonUnitIndex{ 0 };
    int waveDataIndex{ 0 };

    bool isWaveActivated = false;

    float m_elapsed{ 0.0f };

    std::vector<UnitProductor*> productorSelector;

    bool isStoppedByTacticMode{ false };
    bool isAllUnitTerminated;

public:
    static std::weak_ptr<PlaytimeWave> GetCurrentOperatingWave();
	static std::weak_ptr<PlaytimeWave> currentOperativeWave;
	std::vector<Unit*> m_currentWaveUnitVector;
    Unit* inGameUnit;
    virtual ~PlaytimeWave();

    bool IsRemainEnemyAndWave();

    void ActivateWave();
    void DeActivateWave();

    virtual void Start() override;
    virtual void Update() override;
    application::editor::WaveData* waveData{ nullptr };

    void StopWaveElapsedTime();
    void ResumeWaveElapsedTime();
    virtual Component* GetComponent() override { return this; }
};