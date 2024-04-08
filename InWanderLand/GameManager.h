#pragma once
#include "YunutyEngine.h"
#include "Unit.h"

class RTSCam;

class GameManager : public Component, public SingletonComponent<GameManager>
{
private:
	bool isBattleModeOn = false;

public:
	virtual void Start() override;
	virtual void Update() override;

	void EngageBattle();
	void EndBattle();

	void Reset();

	bool IsBattleSystemOperating() const;

	RTSCam* rtscam;
};

