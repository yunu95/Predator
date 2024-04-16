#pragma once
#include "YunutyEngine.h"

class RTSCam;

class GameManager : public Component, public SingletonComponent<GameManager>
{
private:
	bool isBattleModeOn = false;

	int currentCombo{ 0 };

	bool isComboTimerActivated{ false };
	float m_comboElapsed;
	float m_comboResistDuration{6.0f};

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

	RTSCam* rtscam;
};

