#include "GameManager.h"


void GameManager::EngageBattle()
{
	isBattleModeOn = true;
}

void GameManager::EndBattle()
{
	isBattleModeOn = false;
}

bool GameManager::IsBattleSystemOperating() const
{
	return isBattleModeOn;
}
