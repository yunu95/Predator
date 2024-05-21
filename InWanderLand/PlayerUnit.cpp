#include "PlayerUnit.h"
#include "GameManager.h"
#include "PlayerController.h"
#include "SkillUnit.h"

void PlayerUnit::Start()
{
	SkillUnit::Start();
	
	/// Player에게만 적용되는 fsm 로직을 추가합니다.

	unitFSM.engageAction[UnitState::OffsetMove] = [this]() { OffsetMoveEngage(); };
	unitFSM.engageAction[UnitState::WaveStart] = [this]() { WaveStartEngage(); };
	unitFSM.engageAction[UnitState::WaveMotion] = [this]() { WaveMotionEngage(); };
	unitFSM.engageAction[UnitState::Resurrect] = [this]() { ResurrectEngage(); };

	unitFSM.updateAction[UnitState::OffsetMove] = [this]() { OffsetMoveUpdate(); };
	unitFSM.updateAction[UnitState::WaveStart] = [this]() { WaveStartUpdate(); };
	unitFSM.updateAction[UnitState::WaveMotion] = [this]() { WaveMotionUpdate(); };
	unitFSM.updateAction[UnitState::Resurrect] = [this]() { ResurrectUpdate(); };

	/// OffsetMove
	unitFSM.transitions[UnitState::OffsetMove].push_back({ UnitState::WaveStart,
		[this]() { return GameManager::Instance().IsPlayerJustEnteredWaveRegion(); } });

	/// WaveStart
	unitFSM.transitions[UnitState::WaveStart].push_back({ UnitState::WaveMotion,
		[this]() { return GameManager::Instance().IsReadyToWaveEngageMotion(); } });

	/// WaveMotion
	unitFSM.transitions[UnitState::WaveMotion].push_back({ UnitState::Idle,
		[this]() { return isWaveMotionEnded; } });

	/// Resurrect
	unitFSM.transitions[UnitState::Resurrect].push_back({ UnitState::Idle,
		[this]() { return isResurrectEnded; } });
}

void PlayerUnit::OffsetMoveEngage()
{
	StopMove();
	StopAllStateTimer();
	currentOrder = UnitState::OffsetMove;
	isFollowing = false;
	m_navAgentComponent->SetSpeed(m_speed);
	ChangeAnimation(unitAnimations.m_idleAnimation);

	m_staticMeshRenderer->GetGI().GetMaterial()->SetColor(yunuGI::Color::green());
}

void PlayerUnit::WaveStartEngage()
{
	if (!unitStatusUI.expired())
		unitStatusUI.lock()->EnableElement();
	StopAllStateTimer();
	currentOrder = UnitState::WaveStart;
	ChangeAnimation(unitAnimations.m_walkAnimation);
	m_navAgentComponent->MoveTo(m_waveStartPosition);
}

void PlayerUnit::WaveMotionEngage()
{
	StopAllStateTimer();
	currentOrder = UnitState::WaveMotion;
	isWaveMotionEnded = false;
	ChangeAnimation(unitAnimations.m_battleEngageAnimation);

	m_stateTimerVector[UnitState::WaveMotion]->pushDuration = unitAnimations.m_battleEngageAnimation->GetDuration() + rotateTime;
	m_stateTimerVector[UnitState::WaveMotion]->onExpiration = [this]()
		{
			isWaveMotionEnded = true;
			GameManager::Instance().ReportWaveMotionEnd(this);
		};
	m_stateTimerVector[UnitState::WaveMotion]->ActivateTimer();
}


void PlayerUnit::ResurrectEngage()
{
	StopAllStateTimer();
	currentOrder = UnitState::Resurrect;
	isResurrectEnded = false;
	deathFunctionElapsed = 0.0f;
	ChangeAnimation(unitAnimations.m_deathAnimation);
	m_opponentObjectSet.clear();
	ReportUnitDeath();
	GetGameObject()->GetComponent<physics::Collider>()->SetActive(false);
	StopMove();

	m_stateTimerVector[UnitState::Resurrect]->pushDuration = m_resurrectingDuration;
	m_stateTimerVector[UnitState::Resurrect]->onExpiration = [this]()
		{
			isResurrectEnded = true;
			GetGameObject()->GetComponent<physics::Collider>()->SetActive(true);
			SetUnitCurrentHp(m_maxHealthPoint);
			ChangeUnitStatRandomly();
		};
	m_stateTimerVector[UnitState::Resurrect]->ActivateTimer();
}

void PlayerUnit::OffsetMoveUpdate()
{
	/// m_followingTargetUnit 과의 거리가 일정 수치 이상으로 벌어진다면 이동 함수 호출.
  /// 다시 거리가 좁혀지면 StopMove. (Idle로 상태 변경)
	if (m_followingTargetUnit)
	{
		float betweenUnitDistance = (m_followingTargetUnit->GetTransform()->GetWorldPosition() - GetTransform()->GetWorldPosition()).Magnitude();

		float distance = (m_currentMovePosition - GetTransform()->GetWorldPosition()).Magnitude();

		if (betweenUnitDistance >= m_followEngageDinstance)
		{
			ChangeAnimation(unitAnimations.m_walkAnimation);

			dotween->DOLookAt(m_followingTargetUnit->GetTransform()->GetWorldPosition(), rotateTime, false);
			m_navAgentComponent->MoveTo(m_followingTargetUnit->GetTransform()->GetWorldPosition());
			isFollowing = true;
		}
		else if (betweenUnitDistance <= m_stopFollowDinstance)
		{
			isFollowing = false;
			StopMove();
			ChangeAnimation(unitAnimations.m_idleAnimation);
		}
		else if (betweenUnitDistance <= m_followEngageDinstance && betweenUnitDistance >= m_stopFollowDinstance && isFollowing)
		{
			ChangeAnimation(unitAnimations.m_walkAnimation);

			dotween->DOLookAt(m_followingTargetUnit->GetTransform()->GetWorldPosition(), rotateTime, false);
			m_navAgentComponent->MoveTo(m_followingTargetUnit->GetTransform()->GetWorldPosition());
			isFollowing = true;
		}
	}
}

void PlayerUnit::WaveStartUpdate()
{
	if (abs(GetGameObject()->GetTransform()->GetWorldPosition().x - m_waveStartPosition.x) < 0.2f && abs(GetGameObject()->GetTransform()->GetWorldPosition().z - m_waveStartPosition.z) < 0.2f)
	{
		StopMove();
		GameManager::Instance().ReportWaveStartStateEnd(this);
	}
}

void PlayerUnit::WaveMotionUpdate()
{

}

void PlayerUnit::ResurrectUpdate()
{

}

void PlayerUnit::ReportLeaderUnitChanged(UnitType p_type)
{
	if (m_unitType == p_type)
	{
		SetUnitStateDirectly(UnitState::Idle);
		m_followingTargetUnit = nullptr;
	}
	else
	{
		m_followingTargetUnit = PlayerController::Instance().GetPlayerMap().find(p_type)->second;
		SetUnitStateDirectly(UnitState::OffsetMove);
	}
}