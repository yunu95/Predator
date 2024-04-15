#include "EnemySummonSkillSystem.h"
#include "MeleeEnemyProductor.h"
#include "RangedEnemyProductor.h"
#include "ShortcutSystem.h"
#include "Unit.h"

void EnemySummonSkillSystem::Start()
{
	m_elapsed = 0.0f;
	m_enemySpawnDuration = 10.0f;
	isThisUnitSummoned = true;
	SetOtherComponentsAsMember();
}

void EnemySummonSkillSystem::Update()
{
	if (isThisUnitSummoned)
	{
		m_elapsed += Time::GetDeltaTime();
		
		if (m_elapsed >= m_enemySpawnDuration)
		{
			ActivateSkillOne();
			m_elapsed = 0.0f;
		}
	}
}

void EnemySummonSkillSystem::ActivateSkill(Unit::SkillEnum p_currentSkill, Vector3d p_skillPosition)
{

}

void EnemySummonSkillSystem::ActivateSkillOne()
{
	Vector3d summonPosition = GetTransform()->GetWorldPosition() + GetTransform()->GetWorldRotation().Forward() * -5.0f;
	
	/// 예시로 근접 유닛 두 마리, 원거리 유닛 1마리를 생성해보자.
	
	//UnitObjectPool::SingleInstance().ChooseProductor(&RangedEnemyProductor::Instance());
	//UnitObjectPool::SingleInstance().SetStartPosition(summonPosition);
	//m_currentSummonedUnit = UnitObjectPool::SingleInstance().Borrow()->m_pairUnit;
	//application::ShortcutSystem::Instance().RegisterTriggerFunction(2,
	//	[=]() { m_currentSummonedUnit->GetGameObject()->SetSelfActive(!m_currentSummonedUnit->GetGameObject()->GetSelfActive()); });

	//UnitObjectPool::SingleInstance().ChooseProductor(&MeleeEnemyProductor::Instance());
	//UnitObjectPool::SingleInstance().SetStartPosition(summonPosition + GetTransform()->GetWorldRotation().Right() * 3.0f);
	//m_currentSummonedUnit = UnitObjectPool::SingleInstance().Borrow()->m_pairUnit;
	//application::ShortcutSystem::Instance().RegisterTriggerFunction(2,
	//	[=]() { m_currentSummonedUnit->GetGameObject()->SetSelfActive(!m_currentSummonedUnit->GetGameObject()->GetSelfActive()); });

	//UnitObjectPool::SingleInstance().ChooseProductor(&MeleeEnemyProductor::Instance());
	//UnitObjectPool::SingleInstance().SetStartPosition(summonPosition + GetTransform()->GetWorldRotation().Right() * -3.0f);
	//m_currentSummonedUnit = UnitObjectPool::SingleInstance().Borrow()->m_pairUnit;
	//application::ShortcutSystem::Instance().RegisterTriggerFunction(2,
	//	[=]() { m_currentSummonedUnit->GetGameObject()->SetSelfActive(!m_currentSummonedUnit->GetGameObject()->GetSelfActive()); });
}
