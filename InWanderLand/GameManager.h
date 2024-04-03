#pragma once
#include "YunutyEngine.h"
#include "Unit.h"

class GameManager : public Component, public SingletonComponent<GameManager>
{
private:
	bool isBattleModeOn = false;

public:
	void EngageBattle();
	void EndBattle();

	bool IsBattleSystemOperating() const;
};

