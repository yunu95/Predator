//#include "GameManager.h"
//#include "PlayerController.h"
//#include "SkillPreviewSystem.h"
//#include "Unit.h"
//#include "PlayerUnit.h"
//#include "RTSCam.h"
//#include "ContentsLayer.h"
//#include "Application.h"
//#include "InputManager.h"
//#include "PlayTimeWave.h"
//#include "UIElement.h"
//#include "InWanderLand.h"
//
//void GameManager::Start()
//{
//	//application::contents::ContentsLayer* contentsLayer = dynamic_cast<application::contents::ContentsLayer*>(application::Application::GetInstance().GetContentsLayer());
//	//contentsLayer->RegisterToEditorComponentVector(this);
//
//	//EndBattle();
//}
//
//void GameManager::Update()
//{
//	if (isComboTimerActivated)
//	{
//		m_comboElapsed += Time::GetDeltaTime();
//
//		if (m_comboElapsed >= m_comboResistDuration)
//		{
//			isComboTimerActivated = false;
//			m_comboElapsed = 0.0f;
//			currentCombo = 0;
//			/// member로 세팅해놨던 comboNumber UIImage Component에게 현재 콤보 수를 넘겨준다.
//			ReportComboChanged();
//		}
//	}
//}
//
//void GameManager::OnContentsStop()
//{
//	this->SetActive(false);
//	m_waveEnterCheckMap.clear();
//	m_waveEnterMotionCheckMap.clear();
//	currentActivatingWave = nullptr;
//	isBattleModeOn = false;
//	currentCombo = 0;
//	isComboTimerActivated = false;
//	m_comboElapsed = 0.0f;
//	m_comboResistDuration = 6.0f;
//	isPlayerEnteredWaveRegion = false;
//	waveEngageMotionActivate = false;
//	waveEngageMotionEnd = false;
//
//	ReportComboChanged();
//}
//
//void GameManager::EngageBattle()
//{
//	isBattleModeOn = true;
//
//	/// 플레이어 유닛들을 WaveEngage 상태로 전환 시키는 로직
//	PlayerController::Instance().SetCurrentPlayerSerialNumber(Unit::UnitType::Warrior);
//	rtscam->SetTarget(PlayerController::Instance().GetPlayerMap().find(Unit::UnitType::Warrior)->second->GetGameObject());
//}
//
//void GameManager::EndBattle()
//{
//	isBattleModeOn = false;
//
//	PlayerController::Instance().ReportBattleEnded();
//	rtscam->SetTarget(PlayerController::Instance().GetPlayerMap().find(Unit::UnitType::Warrior)->second->GetGameObject());
//	//SkillPreviewSystem::Instance().ActivateSkillPreview(false);
//}
//
//void GameManager::EngageCinematic()
//{
//	InputManager::Instance().SetInputManagerActive(false);
//	//SkillPreviewSystem::Instance().ActivateSkillPreview(false);
//}
//
//void GameManager::EndCinematic()
//{
//#ifdef EDITOR
//	if (!application::Application::GetInstance().IsContentsPlaying())
//	{
//		return;
//	}
//#endif
//
//	InputManager::Instance().SetInputManagerActive(true);
//	//SkillPreviewSystem::Instance().ActivateSkillPreview(false);
//
//	/// PlayerController 에서
//
//	/// [1] Battle 일 경우, 
//	/// Leader 가 아닌 녀석들만 Idle 상태로 바꿔주는 로직
//
//	/// [2] Battle 아닐 경우,
//	/// 살아있는 녀석 중 빠른 녀석 혹은 선택된 녀석 Leader 로 세팅하고
//	/// 나머지 녀석들은 OffsetMove 로 변경
//	if (isBattleModeOn)
//	{
//		PlayerUnit* tempLeaderUnit = PlayerController::Instance().GetCurrentSelectedPlayerUnit();
//		
//		for (auto e : PlayerController::Instance().GetPlayerMap())
//		{
//			if (e.second == tempLeaderUnit)
//				continue;
//			e.second->SetUnitStateDirectly(Unit::UnitState::Idle);
//		}
//	}
//	else
//	{
//		PlayerController::Instance().ReportBattleEnded();
//	}
//}
//
//void GameManager::Reset()
//{
//	isBattleModeOn = false;
//	PlayerController::Instance().SetCurrentPlayerSerialNumber(Unit::UnitType::Warrior);
//	rtscam->SetTarget(PlayerController::Instance().GetPlayerMap().find(Unit::UnitType::Warrior)->second->GetGameObject());
//}
//
//bool GameManager::IsBattleSystemOperating() const
//{
//	return isBattleModeOn;
//}
//
//void GameManager::AddCombo()
//{
//	isComboTimerActivated = true;
//	m_comboElapsed = 0.0f;
//	currentCombo++;
//	/// member로 세팅해놨던 comboNumber UIImage Component에게 현재 콤보 수를 넘겨준다.
//	ReportComboChanged();
//}
//
//void GameManager::ResetCombo()
//{
//	currentCombo = 0;
//	ReportComboChanged();
//}
//
//void GameManager::ReportComboChanged()
//{
//	/// 콤보 수가 변경될 때마다 호출되는 함수입니다.
//	/// 이 함수에서 멤버변수 currentCombo 를 comboNumber UIImage Component 에게 전달하면 됩니다.
//	currentCombo++;
//	if (currentCombo == 0)
//	{
//		UIManager::Instance().GetUIElementByEnum(UIEnumID::Ingame_Combo_Number)->DisableElement();
//		UIManager::Instance().GetUIElementByEnum(UIEnumID::Ingame_Combo_Text)->DisableElement();
//		//UIManager::Instance().GetUIElementByEnum(UIEnumID::Ingame_Combo_Number)->SetNumber(currentCombo);
//	}
//	else
//	{
//		UIManager::Instance().GetUIElementByEnum(UIEnumID::Ingame_Combo_Number)->EnableElement();
//		UIManager::Instance().GetUIElementByEnum(UIEnumID::Ingame_Combo_Text)->EnableElement();
//		UIManager::Instance().GetUIElementByEnum(UIEnumID::Ingame_Combo_Number)->SetNumber(currentCombo);
//	}
//	for (auto i = 0; i < 3; i++)
//	{
//		if (!comboAchieved[i] && comboObjective[i] > 0 && currentCombo >= comboObjective[i])
//		{
//			comboAchieved[i] = true;
//			SkillUpgradeSystem::SingleInstance().IncrementSkillPoint();
//			UIManager::Instance().GetUIElementByEnum(UIManager::comboCheckImgs[i])->EnableElement();
//			UIManager::Instance().GetUIElementByEnum(UIManager::comboFinishedImgs[i])->EnableElement();
//			UIManager::Instance().GetUIElementByEnum(UIManager::comboUnFinishedImgs[i])->DisableElement();
//		}
//	}
//}
//
//void GameManager::ReportWaveStartStateEnd(Unit* p_unit)
//{
//	m_waveEnterCheckMap.find(p_unit->GetUnitType())->second = true;
//
//	bool tempCheck = true;
//
//	for (auto each : m_waveEnterCheckMap)
//	{
//		if (each.second == false)
//		{
//			tempCheck = false;
//			break;
//		}
//	}
//
//	if (tempCheck)
//	{
//		/// 모든 플레이어 유닛이 wave진입 완료
//		waveEngageMotionActivate = true;
//		isPlayerEnteredWaveRegion = false;
//		waveEngageMotionEnd = false;
//		m_waveEnterCheckMap.clear();
//		EngageBattle();
//	}
//}
//
//void GameManager::ReportWaveMotionEnd(Unit* p_unit)
//{
//	m_waveEnterMotionCheckMap.insert({ Unit::UnitType::Warrior, false });
//	m_waveEnterMotionCheckMap.insert({ Unit::UnitType::Magician, false });
//	m_waveEnterMotionCheckMap.insert({ Unit::UnitType::Healer, false });
//
//	m_waveEnterMotionCheckMap.find(p_unit->GetUnitType())->second = true;
//
//	bool tempCheck = true;
//
//	for (auto each : m_waveEnterMotionCheckMap)
//	{
//		if (each.second == false)
//		{
//			tempCheck = false;
//			break;
//		}
//	}
//
//	if (tempCheck)
//	{
//		/// 모든 플레이어 유닛이 wave Motion 완료
//		waveEngageMotionEnd = true;
//		m_waveEnterMotionCheckMap.clear();
//	}
//}
//
//void GameManager::ReportPlayerEnteredWaveRegion(PlaytimeWave* p_wave)
//{
//	waveEngageMotionActivate = false;
//	isPlayerEnteredWaveRegion = true;
//	currentActivatingWave = p_wave;
//	m_waveEnterCheckMap.insert({ Unit::UnitType::Warrior, false });
//	m_waveEnterCheckMap.insert({ Unit::UnitType::Magician, false });
//	m_waveEnterCheckMap.insert({ Unit::UnitType::Healer, false });
//
//	Unit* currentLeaderUnit = PlayerController::Instance().GetCurrentSelectedPlayerUnit();
//
//	yunutyEngine::Transform* leaderUnitTransform = currentLeaderUnit->GetTransform();
//	Vector3d leftPosition = leaderUnitTransform->GetWorldPosition() + leaderUnitTransform->GetWorldRotation().Right() * -3.0f;
//	Vector3d rightPosition = leaderUnitTransform->GetWorldPosition() + leaderUnitTransform->GetWorldRotation().Right() * 3.0f;
//
//	currentLeaderUnit->SetWaveStartPosition(leaderUnitTransform->GetWorldPosition());
//	currentLeaderUnit->m_currentBelongingWavePosition = p_wave->GetTransform()->GetWorldPosition();
//
//	/// 유닛들을 warrior 기준 양쪽으로 이동시키자
//	int temp = 1;
//
//	for (auto e : PlayerController::Instance().GetPlayerMap())
//	{
//		if (e.second == currentLeaderUnit)
//			continue;
//
//		if (temp > 0)
//		{
//			e.second->SetWaveStartPosition(leftPosition);
//			temp *= -1;
//		}
//		else
//		{
//			e.second->SetWaveStartPosition(rightPosition);
//		}
//		e.second->m_currentBelongingWavePosition = p_wave->GetTransform()->GetWorldPosition();
//	}
//}
//
//bool GameManager::IsPlayerJustEnteredWaveRegion() const
//{
//	return isPlayerEnteredWaveRegion;
//}
//
//bool GameManager::IsReadyToWaveEngageMotion() const
//{
//	return waveEngageMotionActivate;
//}
//
//bool GameManager::IsWaveEngageMotionEnd() const
//{
//	return waveEngageMotionEnd;
//}
//void GameManager::SetComboObjectives(const array<int, 3>& comboObjectives)
//{
//	for (int i = 0; i < 3; i++)
//	{
//		comboAchieved[i] = false;
//		comboObjective[i] = comboObjectives[i];
//		UIManager::Instance().GetUIElementByEnum(UIManager::comboNumbers[i])->SetNumber(comboObjectives[i]);
//		UIManager::Instance().GetUIElementByEnum(UIManager::comboNumbers[i + 3])->SetNumber(comboObjectives[i]);
//		UIManager::Instance().GetUIElementByEnum(UIManager::comboFinishedImgs[i])->DisableElement();
//		UIManager::Instance().GetUIElementByEnum(UIManager::comboUnFinishedImgs[i])->EnableElement();
//		UIManager::Instance().GetUIElementByEnum(UIManager::comboCheckImgs[i])->DisableElement();
//	}
//}
