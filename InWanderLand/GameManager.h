#pragma once
#include "YunutyEngine.h"

class RTSCam;
class PlaytimeWave;

class GameManager : public Component, public SingletonComponent<GameManager>
{
private:
	bool isBattleModeOn = false;

	int currentCombo{ 0 };

	bool isComboTimerActivated{ false };
	float m_comboElapsed;
	float m_comboResistDuration{6.0f};

	bool isPlayerEnteredWaveRegion{ false };

	PlaytimeWave* currentActivatingWave;

	std::unordered_map<Unit::UnitType, bool> m_waveEnterCheckMap;
	std::unordered_map<Unit::UnitType, bool> m_waveEnterMotionCheckMap;
	bool waveEngageMotionActivate{ false };

	bool waveEngageMotionEnd{ false };

public:
	virtual void Start() override;
	virtual void Update() override;

	void EngageBattle();
	void EndBattle();

	void EngageCinematic();
	void EndCinematic();

	void Reset();

	bool IsBattleSystemOperating() const;

	void AddCombo();
	void ResetCombo();

	void ReportWaveStartStateEnd(Unit* p_unit);
	void ReportWaveMotionEnd(Unit* p_unit);

	void ReportPlayerEnteredWaveRegion(PlaytimeWave* p_wave);
	bool IsPlayerJustEnteredWaveRegion() const;
	bool IsReadyToWaveEngageMotion() const;
	bool IsWaveEngageMotionEnd() const;
	RTSCam* rtscam;
};

