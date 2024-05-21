#include "SkillUnit.h"
#include "SkillSystem.h"
#include "TacticModeSystem.h"
#include "PlayerController.h"
#include "BossSkillSystem.h"

void SkillUnit::Start()
{
	Unit::Start();

	/// Skill
	unitFSM.transitions[UnitState::Skill].push_back({ UnitState::Idle,
		[=]() { return currentOrder == UnitState::Idle; } });

	for (int i = static_cast<int>(UnitState::Idle); i < static_cast<int>(UnitState::Skill); i++)
	{
		unitFSM.transitions[static_cast<UnitState>(i)].push_back({ UnitState::Skill,
		[=]() { return currentOrder == UnitState::Skill /*&& TacticModeSystem::Instance().IsOrderingTimingNow()*/; } });
	}

	unitFSM.engageAction[UnitState::Skill] = [this]() { SkillEngage(); };
	unitFSM.updateAction[UnitState::Skill] = [this]() { SkillUpdate(); };
}

void SkillUnit::SkillEngage()
{
	currentOrder = UnitState::Skill;
	ChangeAnimation(unitAnimations.m_idleAnimation);
	isSkillUsed = false;
	skillFunctionStartElapsed = 0.0f;
	int tempRand = rand() % 3 + 1;

	if (m_unitType == UnitType::Boss)
	{
		auto temp = GetGameObject()->GetComponent<BossSkillSystem>();
		temp->SelectSkillRandomly();
		m_currentSelectedSkill = temp->GetCurrentSelectedSkillNumber();
		if (m_currentTargetUnit != nullptr)
			m_currentSkillPosition = m_currentTargetUnit->GetTransform()->GetWorldPosition();
		else
			m_currentSkillPosition = PlayerController::Instance().GetPlayerMap().find(static_cast<UnitType>(tempRand))->second->GetTransform()->GetWorldPosition();
	}
	else if (m_unitType == UnitType::SpikeTrap || m_unitType == UnitType::ChessTrap)
	{
		m_currentSelectedSkill = SkillEnum::BossSkillOne;
	}
	else if (m_unitType == UnitType::MeleeEnemy)
	{
		m_currentSelectedSkill = SkillEnum::BossSkillOne;
		if (m_currentTargetUnit != nullptr)
			m_currentSkillPosition = m_currentTargetUnit->GetTransform()->GetWorldPosition();
		else
			m_currentSkillPosition = PlayerController::Instance().GetPlayerMap().find(static_cast<UnitType>(tempRand))->second->GetTransform()->GetWorldPosition();
	}
	else if (m_unitType == UnitType::RangedEnemy)
	{
		m_currentSelectedSkill = SkillEnum::BossSkillTwo;
		if (m_currentTargetUnit != nullptr)
			m_currentSkillPosition = m_currentTargetUnit->GetTransform()->GetWorldPosition();
		else
			m_currentSkillPosition = PlayerController::Instance().GetPlayerMap().find(static_cast<UnitType>(tempRand))->second->GetTransform()->GetWorldPosition();
	}

	if (m_unitType != UnitType::SpikeTrap && m_unitType != UnitType::ChessTrap)
		dotween->DOLookAt(m_currentSkillPosition, rotateTime, false);

	if (!m_skillDurationMap.empty())
	{
		m_currentSelectedSkillEngageDelay = m_skillDurationMap.find(m_currentSelectedSkill)->second;
	}
	if (!m_skillAnimationMap.empty())
	{
		m_currentSkillAnimation = m_skillAnimationMap.find(m_currentSelectedSkill)->second;
	}
	if (!m_skillTimingFrameMap.empty())
	{
		m_selectedSkillTimingFrame = m_skillTimingFrameMap.find(m_currentSelectedSkill)->second;
	}

	StopMove();
}

void SkillUnit::SkillUpdate()
{
	if (!isSkillUsed)
	{
		skillFunctionStartElapsed += Time::GetDeltaTime() * m_localTimeScale;

		if (skillFunctionStartElapsed >= m_currentSelectedSkillEngageDelay)
		{
			isSkillUsed = true;
			skillFunctionStartElapsed = 0.0f;
			ChangeAnimation(m_currentSkillAnimation);
		}
	}
}

void SkillUnit::RegisterSkillWithAnimation(SkillEnum p_enum)
{
	yunuGI::IAnimation* temp{ nullptr };

	switch (p_enum)
	{
	case Unit::SkillEnum::Q:
		temp = unitAnimations.m_skillOneAnimation;
		break;
	case Unit::SkillEnum::W:
		temp = unitAnimations.m_skillTwoAnimation;
		break;
	case Unit::SkillEnum::BossSkillOne:
		temp = unitAnimations.m_skillOneAnimation;
		break;
	case Unit::SkillEnum::BossSkillTwo:
		temp = unitAnimations.m_skillTwoAnimation;
		break;
	case Unit::SkillEnum::BossSkillThree:
		temp = unitAnimations.m_skillThreeAnimation;
		break;
	case Unit::SkillEnum::BossSkillFour:
		temp = unitAnimations.m_skillFourAnimation;
		break;
	default:
		break;
	}

	if (SkillSystem* skillsys = GetGameObject()->GetComponent<SkillSystem>();
		skillsys && temp)
	{
		m_animatorComponent = GetGameObject()->GetComponent<yunutyEngine::graphics::Animator>();

		m_animatorComponent->PushAnimationWithFunc(temp, m_skillTimingFrameMap.find(p_enum)->second, [=]()
			{
				skillsys->ActivateSkill(p_enum, m_currentSkillPosition);
			});
	}
}