#include "TacticModeSystem.h"
#include "Unit.h"
#include "InputManager.h"
#include "RTSCam.h"
#include "PlayerController.h"
#include "SkillPreviewSystem.h"
#include "CursorDetector.h"
#include "PlaytimeWave.h"
#include "WaveData.h"
#include "LocalTimeEntityManager.h"
#include "PlayerSkillSystem.h"
#include "UnitCommand.h"
#include "InWanderLand.h"

//void TacticModeSystem::OnEnable()
//{
//	//AddGauge(m_maxGauge);
//}
//
//void TacticModeSystem::Start()
//{
//	//AddGauge(m_maxGauge);
//
//	//isCoolTime = false;
//	//SetCurrentCoolTimeElapsed(0.0f);
//
//	//gaugeRecoveryPerSecond = application::GlobalConstant::GetSingletonInstance().pod.skillGaugeRecoveryPerSecond;
//	//skillCost = application::GlobalConstant::GetSingletonInstance().pod.tacticSkillCost;
//	//moveCost = application::GlobalConstant::GetSingletonInstance().pod.tacticMoveCost;
//	//attackCost = application::GlobalConstant::GetSingletonInstance().pod.tacticAttackCost;
//}
//
//void TacticModeSystem::Update()
//{
//	//if (!isTacticModeOperating)
//	//{
//	//	AddGauge(gaugeRecoveryPerSecond * Time::GetDeltaTime());
//	//}
//
//	//if (isCoolTime)
//	//{
//	//	SetCurrentCoolTimeElapsed(m_engageCoolTimeElapsed + Time::GetDeltaTime());
//	//	if (m_engageCoolTimeElapsed >= m_engageCoolTimeDuration)
//	//	{
//	//		SetCurrentCoolTimeElapsed(m_engageCoolTimeDuration);
//	//		isCoolTime = false;
//	//	}
//	//}
//}
//
//void TacticModeSystem::OnContentsStop()
//{
//	//m_currentWaveUnits.clear();
//	//playerComponentMap.clear();
//
//	//for (int i = 0; i < sequenceQueue.size(); i++)
//	//{
//	//	sequenceQueue.pop_front();
//	//}
//
//	//m_maxGauge = 10;
//	//m_currentGauge = 0;
//
//	//isCoolTime = false;
//	//m_engageCoolTimeDuration = 5.0f;
//	//m_engageCoolTimeElapsed = 0.0f;
//
//	//isTacticModeOperating = false;
//	//isTacticOrderPerforming = false;
//	//currentSelectedUnit = nullptr;
//	////m_currentOperatingWave = nullptr;
//
//	//this->SetActive(false);
//}
//
//void TacticModeSystem::ToggleRequested(InputManager::SelectedSerialNumber currentSelectedNum)
//{
//	m_latestSelectedUnitNum = currentSelectedNum;
//	if (m_latestSelectedUnitNum == 0)
//		m_latestSelectedUnitNum = InputManager::One;
//
//	if (!isTacticOrderPerforming)           /// 1. 명령 실행 중이 아니고,
//	{
//		if (!isCoolTime)
//		{
//			if (!isTacticModeOperating)     /// 2. 명령 입력 중이 아니면 진입.
//			{
//				EngageTacticMode();
//				SetTacticModeRightClickFunction(currentSelectedNum);
//			}
//			else                            /// 3. 명령 입력 중이라면 나가기.
//			{
//				//StartCoroutine([](TacticModeSystem* tac)->coroutine::Coroutine 
//				//	{
//				//	tac->isReady = false;
//				//	co_yield coroutine::WaitForSeconds{ 3 };
//				//	tac->isReady = true;
//				//	co_return;
//				//	}(this));
//
//				ExitTacticMode();
//				PlayerController::Instance().SetCurrentPlayerSerialNumber(static_cast<Unit::UnitType>(currentSelectedNum));
//				PlayerController::Instance().SetRightClickFunction();
//			}
//		}
//	}
//}
//
//void TacticModeSystem::SetTacticModeRightClickFunction(InputManager::SelectedSerialNumber currentSelectedNum)
//{
//	if (currentSelectedNum == 0)
//		currentSelectedNum = InputManager::SelectedSerialNumber::One;
//
//	currentSelectedUnit = playerComponentMap.find(static_cast<Unit::UnitType>(currentSelectedNum))->second;
//
//	m_rtsCam->groundRightClickCallback = [=](Vector3d pos)
//		{
//			if (m_cursorDetector->GetCurrentOnMouseUnit() == nullptr)
//			{
//				if (m_currentGauge - moveCost > 0)
//				{
//					// 오브젝트의 현재 포지션에서 이동할 위치까지의 경로 메쉬를 만들고 보여줌
//					const auto& previewQueue = currentSelectedUnit->GetTacticPreview();
//					auto field = currentSelectedUnit->GetNavField();
//					TacticPreview preview;
//					std::vector<Vector3d> pathVertexList;
//					if (previewQueue.empty())
//					{
//						pathVertexList = field->GetSmoothPath(currentSelectedUnit->GetGameObject()->GetTransform()->GetWorldPosition(), pos);
//					}
//					else
//					{
//						auto preview = previewQueue.back();
//						pathVertexList = field->GetSmoothPath(preview.finalPos, pos);
//					}
//
//					auto pathMesh = SkillPreviewSystem::Instance().ShowRoute(static_cast<SkillPreviewSystem::UnitType>(currentSelectedNum - 1), pathVertexList);
//
//					currentSelectedUnit->PushMoveFunctionToTacticQueue(pos, pathMesh);
//					AddGauge(-1 * moveCost);
//					//sequenceQueue.push(currentSelectedUnit);
//					sequenceQueue.push_back(currentSelectedUnit);
//				}
//			}
//			else if (Unit* selectedUnit = m_cursorDetector->GetCurrentOnMouseUnit();
//				selectedUnit->GetUnitSide() == Unit::UnitSide::Enemy)
//			{
//				if (m_currentGauge - attackCost > 0)
//				{
//					// 평타에 대한 커맨드
//					// 거리가 멀다면 걸어가서 때리기 때문에 이동경로 표시를 해줘야함.
//					// 공격 사거리만큼 vertex 삭제하기
//					const auto& previewQueue = currentSelectedUnit->GetTacticPreview();
//					auto field = currentSelectedUnit->GetNavField();
//					TacticPreview preview;
//					std::vector<Vector3d> pathVertexList;
//
//					if (previewQueue.empty())
//					{
//						pathVertexList = field->GetSmoothPath(currentSelectedUnit->GetGameObject()->GetTransform()->GetWorldPosition(), pos);
//					}
//					else
//					{
//						auto preview = previewQueue.back();
//						pathVertexList = field->GetSmoothPath(preview.finalPos, pos);
//					}
//
//					for (auto it = pathVertexList.rbegin(); it != pathVertexList.rend();)
//					{
//						auto atkDistance = currentSelectedUnit->GetAtkDistance();
//						if (atkDistance >= ((*it) - selectedUnit->GetGameObject()->GetTransform()->GetWorldPosition()).Magnitude() + 0.0001f)
//						{
//							it = std::reverse_iterator(pathVertexList.erase((it + 1).base()));
//						}
//						else
//						{
//							++it;
//						}
//					}
//
//					preview.finalPos = pathVertexList.back();
//
//
//					auto pathMesh = SkillPreviewSystem::Instance().ShowRoute(static_cast<SkillPreviewSystem::UnitType>(currentSelectedNum - 1), pathVertexList);
//
//
//					currentSelectedUnit->PushAttackMoveFunctionToTacticQueue(preview.finalPos, selectedUnit, pathMesh);
//					AddGauge(-1 * attackCost);
//					//sequenceQueue.push(currentSelectedUnit);
//					sequenceQueue.push_back(currentSelectedUnit);
//				}
//			}
//			//SkillPreviewSystem::Instance().ActivateSkillPreview(false);
//		};
//}
//
//void TacticModeSystem::SetLeftClickAddQueueForAttackMove(InputManager::SelectedSerialNumber currentSelectedNum)
//{
//	currentSelectedUnit = playerComponentMap.find(static_cast<Unit::UnitType>(currentSelectedNum))->second;
//
//	m_rtsCam->groundRightClickCallback = [=](Vector3d pos)
//		{
//			if (m_currentGauge > 0)
//			{
//				currentSelectedUnit->PushAttackMoveFunctionToTacticQueue(pos);
//				///sequenceQueue.push(currentSelectedUnit);
//				sequenceQueue.push_back(currentSelectedUnit);
//				//SkillPreviewSystem::Instance().ActivateSkillPreview(false);
//				//m_rtsCam->groundLeftClickCallback = [](Vector3d pos) {};
//				//tacticModeGauge--;
//				//SetTacticModeRightClickFunction(currentSelectedNum);
//				//AddGauge(--m_currentGauge);
//			}
//		};
//}
//
//void TacticModeSystem::SetLeftClickAddQueueForSkill(InputManager::SelectedSerialNumber currentSelectedNum, Unit::SkillEnum currentSelectedSkill)
//{
//	currentSelectedUnit = playerComponentMap.find(static_cast<Unit::UnitType>(currentSelectedNum))->second;
//
//	//SkillPreviewSystem::Instance().SetCurrentSelectedPlayerUnit(currentSelectedUnit);
//	//SkillPreviewSystem::Instance().SetCurrentSkillPreviewType(currentSelectedUnit->GetSkillPreviewType(currentSelectedSkill));
//	//SkillPreviewSystem::Instance().SetCurrentSelectedSkillNum(currentSelectedSkill);
//	//SkillPreviewSystem::Instance().ActivateSkillPreview(true);
//
//	m_rtsCam->groundLeftClickCallback = [=](Vector3d pos)
//		{
//			if (m_currentGauge - skillCost > 0)
//			{
//				// 현재 유닛이 누구인지 어떤 스킬인지에 따라 달라질 것임
//				auto preview = currentSelectedUnit->GetTacticPreview();
//				Vector3d objPos;
//				switch (currentSelectedUnit->GetUnitType())
//				{
//					case Unit::UnitType::Warrior:
//					{
//						if (currentSelectedSkill == Unit::SkillEnum::Q)
//						{
//							// 로빈 Q스킬은 돌진을 함 즉, pos가 나의 위치가 될 것임
//							if (preview.empty())
//							{
//								SkillPreviewSystem::Instance().ShowRobinQSkill(currentSelectedUnit->GetGameObject()->GetTransform()->GetWorldPosition(),
//									currentSelectedUnit->m_playerSkillSystem->GetSkillOneRange());
//								objPos = currentSelectedUnit->GetGameObject()->GetTransform()->GetWorldPosition();
//							}
//							else
//							{
//								SkillPreviewSystem::Instance().ShowRobinQSkill(preview.back().finalPos, currentSelectedUnit->m_playerSkillSystem->GetSkillOneRange());
//								objPos = pos;
//							}
//						}
//						else if (currentSelectedSkill == Unit::SkillEnum::W)
//						{
//							//SkillPreviewSystem::ShowRobin();
//						}
//					}
//					break;
//					//case Unit::UnitType::Magician:
//					//{
//					//	if (currentSelectedSkill == Unit::SkillEnum::Q)
//					//	{
//					//		SkillPreviewSystem::ShowUrsulaQSkill();
//					//	}
//					//	else if (currentSelectedSkill == Unit::SkillEnum::W)
//					//	{
//					//		SkillPreviewSystem::ShowUrsulaWSkill();
//					//	}
//					//}
//					//break;
//					//case Unit::UnitType::Healer:
//					//{
//					//	if (currentSelectedSkill == Unit::SkillEnum::Q)
//					//	{
//					//		SkillPreviewSystem::ShowHanselQSkill();
//					//	}
//					//	else if (currentSelectedSkill == Unit::SkillEnum::W)
//					//	{
//					//		SkillPreviewSystem::ShowHanselWSkill();
//					//	}
//					//}
//					//break;
//					default:
//						break;
//				}
//
//
//				currentSelectedUnit->PushSkillFunctionToTacticQueue(currentSelectedSkill, pos, objPos, currentSelectedSkill);
//				///sequenceQueue.push(currentSelectedUnit);
//				sequenceQueue.push_back(currentSelectedUnit);
//				//SkillPreviewSystem::Instance().ActivateSkillPreview(false);
//				SetTacticModeRightClickFunction(currentSelectedNum);
//				AddGauge(-1 * skillCost);
//			}
//		};
//}
//
//void TacticModeSystem::RegisterTutorialQuestAction(Unit::UnitType p_targetUnit, OrderType p_orderType)
//{
//
//}
//
//void TacticModeSystem::EngageTacticMode()
//{
//	/// 전술모드 키 입력 시 실행될 로직.
//	/// 1. TimeScale을 0으로 설정한다.
//	/// 2. PlayerController에서 현재 전술모드 적용 가능한 Player Unit의 정보를 가져온다.
//	//Time::SetTimeScale(0.0f);
//
//	for (int i = 0; i < sequenceQueue.size(); i++)
//	{
//		sequenceQueue.pop_front();
//	}
//
//	//m_currentOperatingWave->StopWaveElapsedTime();
//	playerComponentMap = PlayerController::Instance().GetPlayerMap();
//	isTacticModeOperating = true;
//
//	LocalTimeEntityManager::Instance().ReportTacticModeEngaged();
//
//	/// 적군 유닛들의 현재 애니메이션도 꺼주자.
//	for (auto each : m_currentWaveUnits)
//	{
//		each->UnitActionOnTacticModeEngaged();
//	}
//
//	for (auto each : playerComponentMap)
//	{
//		each.second->UnitActionOnTacticModeEngaged();
//	}
//
//	vector<GameObject*> unitGameObjects;
//	for (auto each : playerComponentMap)
//	{
//		unitGameObjects.push_back(each.second->GetGameObject());
//	}
//	m_rtsCam->SetTargets(unitGameObjects);
//
//}
//
//void TacticModeSystem::ExitTacticMode()
//{
//	//Time::SetTimeScale(1.0f);    
//
//	/// 플레이어 및 적군의 시간을 Resume합니다. 
//	/// 이 때 플레이어는 입력된 명령을 실제로 실행하고, 적군 유닛은 시간이 멈춘 듯이 설정해줍니다.
//	for (auto each : playerComponentMap)
//	{
//		for (auto e : each.second->m_stateTimerVector)
//		{
//			LocalTimeEntityManager::Instance().SetLocalTimeScaleDirectly(e, 1.0f);
//		}
//
//		LocalTimeEntityManager::Instance().SetLocalTimeScaleDirectly(each.second, 1.0f);
//		LocalTimeEntityManager::Instance().SetLocalTimeScaleDirectly(each.second->GetGameObject()->GetComponent<Dotween>(), 1.0f);
//		each.second->GetGameObject()->GetComponent<PlayerSkillSystem>()->SetSkillRequirmentLocalTimeScale(1.0f);
//	}
//
//	isTacticModeOperating = false;
//
//	for (auto each : playerComponentMap)
//	{
//		if (!each.second->IsTacticModeQueueEmpty())
//			each.second->SetUnitStateIdle();
//	}
//
//	if (sequenceQueue.empty())
//	{
//		SetCurrentCoolTimeElapsed(0.0f);
//		isCoolTime = true;
//		isTacticOrderPerforming = false;
//		LocalTimeEntityManager::Instance().ReportTacticModeEnded();
//		//m_currentOperatingWave->ResumeWaveElapsedTime();
//
//		for (auto each : m_currentWaveUnits)
//		{
//			each->UnitActionOnTacticModeEnded();
//		}
//		//for (auto each : playerComponentMap)
//		//{
//		//	each.second->EnemyActionOnTacticModeEnded();
//		//}
//		m_rtsCam->SetTarget(PlayerController::Instance().GetPlayerMap().find(static_cast<Unit::UnitType>(m_latestSelectedUnitNum))->second->GetGameObject());
//	}
//	else
//	{
//		isTacticOrderPerforming = true;
//		m_rtsCam->SetTarget(sequenceQueue.front()->GetGameObject());
//		sequenceQueue.front()->UnitActionOnTacticModeEnded();
//		sequenceQueue.front()->PermitTacticAction();
//		sequenceQueue.pop_front();
//	}
//}
//
//void TacticModeSystem::SetMovingSystemComponent(RTSCam* sys)
//{
//	m_rtsCam = sys;
//}
//
//bool TacticModeSystem::IsOrderingTimingNow() const
//{
//	return isTacticModeOperating;
//}
//
//bool TacticModeSystem::IsUnitsPerformingCommand() const
//{
//	return isTacticOrderPerforming;
//}
//
//bool TacticModeSystem::IsTacticModeCoolTime() const
//{
//	return isCoolTime;
//}
//
//void TacticModeSystem::SetCurrentCoolTimeElapsed(float elapsed)
//{
//	m_engageCoolTimeElapsed = elapsed;
//	if (isCoolTime)
//	{
//		UIManager::Instance().GetUIElementByEnum(UIEnumID::Toggle_TacticMode_Overlay)->adjuster->SetTargetFloat(elapsed / m_engageCoolTimeDuration);
//		UIManager::Instance().GetUIElementByEnum(UIEnumID::Toggle_TacticMode_Cooltime_Number)->SetNumber(m_engageCoolTimeDuration - elapsed);
//	}
//	else
//	{
//		UIManager::Instance().GetUIElementByEnum(UIEnumID::Toggle_TacticMode_Overlay)->adjuster->SetTargetFloat(1);
//		UIManager::Instance().GetUIElementByEnum(UIEnumID::Toggle_TacticMode_Cooltime_Number)->DisableElement();
//	}
//}
//
//float TacticModeSystem::GetLeftCoolTime()
//{
//	if (m_engageCoolTimeElapsed >= m_engageCoolTimeDuration)
//		return 0.0f;
//	else
//		return m_engageCoolTimeDuration - m_engageCoolTimeElapsed;
//}
//
//void TacticModeSystem::AddGauge(int p_gauge)
//{
//	m_currentGauge += p_gauge;
//
//	if (m_currentGauge < 0)
//	{
//		m_currentGauge = 0;
//	}
//	else if (m_currentGauge > m_maxGauge)
//	{
//		m_currentGauge = m_maxGauge;
//	}
//}
//
//void TacticModeSystem::ReportTacticActionFinished()
//{
//	/// 전술모드의 수행을 완료한 유닛이 해당 함수를 호출합니다.
//	if (sequenceQueue.empty())
//	{
//		SetCurrentCoolTimeElapsed(0.0f);
//		isCoolTime = true;
//		isTacticOrderPerforming = false;
//		LocalTimeEntityManager::Instance().ReportTacticModeEnded();
//		//m_currentOperatingWave->ResumeWaveElapsedTime();
//
//		for (auto each : m_currentWaveUnits)
//		{
//			each->UnitActionOnTacticModeEnded();
//		}
//		for (auto each : playerComponentMap)
//		{
//			each.second->UnitActionOnTacticModeEnded();
//		}
//	}
//	else
//	{
//		m_rtsCam->SetTarget(sequenceQueue.front()->GetGameObject());
//		sequenceQueue.front()->PermitTacticAction();
//		sequenceQueue.pop_front();
//	}
//}
//
//void TacticModeSystem::ShowUnitSkillPreview(Unit* unit, Unit::SkillEnum skillKind)
//{
//
//}
//
//void TacticModeSystem::RegisterCurrentWave(PlaytimeWave* p_wave)
//{
//	//m_currentOperatingWave = p_wave;
//	for (auto each : p_wave->waveData->waveUnitDatasVector)
//	{
//		m_currentWaveUnits.push_back(each->inGameUnit);
//	}
//}
//
//void TacticModeSystem::PopCommand()
//{
//	sequenceQueue.back()->PopCommand();
//	sequenceQueue.pop_back();
//}
//
//void TacticModeSystem::ClearCommand()
//{
//	while (!sequenceQueue.empty())
//	{
//		sequenceQueue.back()->PopCommand();
//		sequenceQueue.pop_back();
//	}
//}
//

void TacticModeSystem::OnEnable()
{

}

void TacticModeSystem::Start()
{
	// 나중에는 글로벌 컨스탄트에서 데이터 받아오기
	this->coolTime = 3.f;
	
}

void TacticModeSystem::Update()
{
	if (isOperating == false && this->isCoolTime == true)
	{
		this->elapsedTime += Time::GetDeltaTime();

		if (this->elapsedTime < this->coolTime)
		{
			this->isCoolTime = true;
		}
		else
		{
			this->isCoolTime = false;
			this->elapsedTime = 0.f;
		}

		if (isCoolTime)
		{
			UIManager::Instance().GetUIElementByEnum(UIEnumID::Toggle_TacticMode_Overlay)->adjuster->SetTargetFloat(this->elapsedTime / this->coolTime);
			UIManager::Instance().GetUIElementByEnum(UIEnumID::Toggle_TacticMode_Cooltime_Number)->EnableElement();
			UIManager::Instance().GetUIElementByEnum(UIEnumID::Toggle_TacticMode_Cooltime_Number)->SetNumber(this->coolTime - this->elapsedTime);
		}
		else
		{
			UIManager::Instance().GetUIElementByEnum(UIEnumID::Toggle_TacticMode_Overlay)->adjuster->SetTargetFloat(1);
			UIManager::Instance().GetUIElementByEnum(UIEnumID::Toggle_TacticMode_Cooltime_Number)->DisableElement();
		}
	}
	else
	{
		UIManager::Instance().GetUIElementByEnum(UIEnumID::Toggle_TacticMode_Overlay)->adjuster->SetTargetFloat(1);
		UIManager::Instance().GetUIElementByEnum(UIEnumID::Toggle_TacticMode_Cooltime_Number)->DisableElement();
	}
}

void TacticModeSystem::OnContentsStop()
{

}

void TacticModeSystem::OnContentsPlay()
{
}

void TacticModeSystem::OperateTacticSystem()
{
	this->isOperating = true;
	this->isCoolTime = true;
}

void TacticModeSystem::Enqueue(std::shared_ptr<UnitCommand> command)
{
	if (isOperating == false) return;

	this->commandQueue.push_back(command);

	// 유닛의 타입에 따라 멤버변수 3개에 알맞게 넣어줘야한다.
}

void TacticModeSystem::Execute()
{
	this->isExecuting = true;
	StartCoroutine(this->ExecuteInternal());
	this->isExecuting = false;
	this->isOperating = false;
}

void TacticModeSystem::PopCommand()
{
	this->commandQueue.back()->SetIsDone(true);
}

void TacticModeSystem::ClearCommand()
{
	for (auto& each : this->commandQueue)
	{
		each->HidePreviewMesh();
	}

	this->commandQueue.clear();
}

yunutyEngine::coroutine::Coroutine TacticModeSystem::ExecuteInternal()
{
	for (auto& each : commandQueue)
	{
		each->Execute();
		while (!each->IsDone())
		{
			co_return;
		}
		each->HidePreviewMesh();
	}

	this->commandQueue.clear();
}
