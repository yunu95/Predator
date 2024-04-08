#include "GameManager.h"
#include "PlayerController.h"
#include "SkillPreviewSystem.h"
#include "Unit.h"
#include "RTSCam.h"
#include "ContentsLayer.h"
#include "Application.h"
#include "InputManager.h"

void GameManager::Start()
{
	application::contents::ContentsLayer* contentsLayer = dynamic_cast<application::contents::ContentsLayer*>(application::Application::GetInstance().GetContentsLayer());
	contentsLayer->RegisterToEditorComponentVector(this);
	EndBattle();
}

void GameManager::Update()
{

}

void GameManager::EngageBattle()
{
	isBattleModeOn = true;		
}

void GameManager::EndBattle()
{
	isBattleModeOn = false;

	PlayerController::SingleInstance().SetCurrentPlayerSerialNumber(Unit::UnitType::AllPlayers);
	rtscam->SetTarget(PlayerController::SingleInstance().GetPlayerMap().find(Unit::UnitType::Warrior)->second->GetGameObject());
	SkillPreviewSystem::Instance().ActivateSkillPreview(false);
}

void GameManager::Reset()
{
	isBattleModeOn = false;
	PlayerController::SingleInstance().SetCurrentPlayerSerialNumber(Unit::UnitType::AllPlayers);
	rtscam->SetTarget(PlayerController::SingleInstance().GetPlayerMap().find(Unit::UnitType::Warrior)->second->GetGameObject());
}

bool GameManager::IsBattleSystemOperating() const
{
	return isBattleModeOn;
}
