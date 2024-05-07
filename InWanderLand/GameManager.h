#pragma once
#include "YunutyEngine.h"
#include "ContentsObservee.h"

class RTSCam;
class PlaytimeWave;

class GameManager : public Component, public SingletonComponent<GameManager>, public ContentsObservee
{
private:

    bool isBattleModeOn = false;

    int currentCombo{ 0 };
    int comboObjective[3]{ 10, 20, 30 };
    bool comboAchieved[3]{ false };

    bool isComboTimerActivated{ false };
    float m_comboElapsed;
    float m_comboResistDuration{ 6.0f };

    bool isPlayerEnteredWaveRegion{ false };

    PlaytimeWave* currentActivatingWave;

    std::unordered_map<Unit::UnitType, bool> m_waveEnterCheckMap;
    std::unordered_map<Unit::UnitType, bool> m_waveEnterMotionCheckMap;
    bool waveEngageMotionActivate{ false };

    bool waveEngageMotionEnd{ false };

public:
    virtual void Start() override;
    virtual void Update() override;

    virtual void PlayFunction() override;
    virtual void StopFunction() override;

    void EngageBattle();
    void EndBattle();

    void EngageCinematic();
    void EndCinematic();

    void Reset();

    bool IsBattleSystemOperating() const;

    void AddCombo();
    void ResetCombo();
    void ReportComboChanged();

    void ReportWaveStartStateEnd(Unit* p_unit);
    void ReportWaveMotionEnd(Unit* p_unit);

    void ReportPlayerEnteredWaveRegion(PlaytimeWave* p_wave);
    bool IsPlayerJustEnteredWaveRegion() const;
    bool IsReadyToWaveEngageMotion() const;
    bool IsWaveEngageMotionEnd() const;
    void SetComboObjectives(const array<int, 3>& comboObjectives);
    RTSCam* rtscam;
};

