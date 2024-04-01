#include "EnemySummonSkillSystem.h"

void EnemySummonSkillSystem::Start()
{
	m_elapsed = 0.0f;
	m_enemySpawnDuration = 10.0f;

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

void EnemySummonSkillSystem::ActivateSkillOne()
{

}
