#include "SpikeSkillSystem.h"

void SpikeSkillSystem::Start()
{
	SetSkillRequirmentsActive(m_spikeDamageRequirement, false);
	SetOtherComponentsAsMember();
	//m_unitComponent->m_currentSelectedSkill
}

void SpikeSkillSystem::Update()
{

}

void SpikeSkillSystem::ActivateSkill(Unit::SkillEnum p_currentSkill, Vector3d p_skillPosition)
{
	SetSkillRequirmentsActive(m_spikeDamageRequirement, true);
	
	m_unitDotween->DONothing(m_colliderRemainDuration).OnComplete([=]()
		{
			SetSkillRequirmentsActive(m_spikeDamageRequirement, false);
			m_unitComponent->SetUnitStateIdle();
		});
}

void SpikeSkillSystem::SetSpikeSkillRequirment(GameObject* p_colliderObj, GameObject* p_debugObj)
{
	m_spikeDamageRequirement.colliderObject = p_colliderObj;
	m_spikeDamageRequirement.debugObject = p_debugObj;
}

void SpikeSkillSystem::ActivateSkillOne()
{

}
