#include "SpikeSkillSystem.h"

void SpikeSkillSystem::Start()
{

}

void SpikeSkillSystem::Update()
{

}

void SpikeSkillSystem::ActivateSkill(Unit::SkillEnum p_currentSkill, Vector3d p_skillPosition)
{
	SetSkillRequirmentsActive(m_spikeDamageRequirement, true);
	
	m_spikeDamageRequirement.dotweenComponent->DONothing(m_spikeTrapDamageDuration).OnComplete([=]()
		{
			SetSkillRequirmentsActive(m_spikeDamageRequirement, false);
			m_unitComponent->SetUnitStateIdle();
		});
}

void SpikeSkillSystem::SetSpikeSkillRequirment(GameObject* p_colliderObj, GameObject* p_debugObj, Dotween* p_dotween)
{
	m_spikeDamageRequirement.colliderObject = p_colliderObj;
	m_spikeDamageRequirement.debugObject = p_debugObj;
	m_spikeDamageRequirement.dotweenComponent = p_dotween;
}

void SpikeSkillSystem::ActivateSkillOne()
{

}
