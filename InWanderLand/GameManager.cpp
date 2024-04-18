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
	if (isComboTimerActivated)
	{
		m_comboElapsed += Time::GetDeltaTime();
		
		if (m_comboElapsed >= m_comboResistDuration)
		{
			isComboTimerActivated = false;
			m_comboElapsed = 0.0f;
			currentCombo = 0;
			/// member로 세팅해놨던 comboNumber UIImage Component에게 현재 콤보 수를 넘겨준다.
		}
	}
}

void GameManager::EngageBattle()
{
	isBattleModeOn = true;

	/// 플레이어 유닛들을 WaveEngage 상태로 전환 시키는 로직
	PlayerController::SingleInstance().SetCurrentPlayerSerialNumber(Unit::UnitType::Warrior);
	rtscam->SetTarget(PlayerController::SingleInstance().GetPlayerMap().find(Unit::UnitType::Warrior)->second->GetGameObject());
}

void GameManager::EndBattle()
{
	isBattleModeOn = false;

	PlayerController::SingleInstance().SetCurrentPlayerSerialNumber(Unit::UnitType::AllPlayers);
	rtscam->SetTarget(PlayerController::SingleInstance().GetPlayerMap().find(Unit::UnitType::Warrior)->second->GetGameObject());
	SkillPreviewSystem::Instance().ActivateSkillPreview(false);
}

void GameManager::EngageCinematic()
{
	InputManager::Instance().SetInputManagerActive(false);
	SkillPreviewSystem::Instance().ActivateSkillPreview(false);
}

void GameManager::EndCinematic()
{
	InputManager::Instance().SetInputManagerActive(true);
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

void GameManager::AddCombo()
{
	isComboTimerActivated = true;
	m_comboElapsed = 0.0f;
	currentCombo++;
	/// member로 세팅해놨던 comboNumber UIImage Component에게 현재 콤보 수를 넘겨준다.
}

void GameManager::ResetCombo()
{
	currentCombo = 0;
}
