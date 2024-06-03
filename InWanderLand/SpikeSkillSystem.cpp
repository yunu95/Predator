//#include "SpikeSkillSystem.h"
//
//void SpikeSkillSystem::Start()
//{
//	SetOtherComponentsAsMember();
//	m_skillUsageDuration = 2.0f;
//
//	m_spikeReachPosition = GetTransform()->GetWorldPosition();
//	m_spikeStartPosition = GetTransform()->GetWorldPosition() - Vector3d(0, 5.0f, 0);
//	GetTransform()->SetWorldPosition(m_spikeStartPosition);
//
//	SetSkillRequirmentsActive(m_spikeDamageRequirement, false);
//	//SetSkillRequirmentsPosition(m_spikeDamageRequirement, m_spikeStartPosition);
//	//m_unitComponent->m_currentSelectedSkill
//}
//
//void SpikeSkillSystem::Update()
//{
//	if (!isSkillOperating)
//	{
//		m_unitComponent->SetUnitStateToSkill();
//		isSkillOperating = true;
//		ActivateSkill(Unit::SkillEnum::BossSkillOne, Vector3d::zero);
//	}
//}
//
//void SpikeSkillSystem::ActivateSkill(Unit::SkillEnum p_currentSkill, Vector3d p_skillPosition)
//{
//	SetSkillRequirmentsActive(m_spikeDamageRequirement, true);
//	
//	m_unitDotween->DOMove(m_spikeReachPosition, 1.0f).OnComplete([=]()
//		{
//			m_unitDotween->DOMove(m_spikeStartPosition, 2.0f).SetDelay(2.0f).OnComplete([=]()
//				{
//					SetSkillRequirmentsActive(m_spikeDamageRequirement, false);
//					isSkillOperating = false;
//				});
//		});
//}
//
//void SpikeSkillSystem::SetSpikeSkillRequirment(GameObject* p_colliderObj, GameObject* p_debugObj)
//{
//	m_spikeDamageRequirement.colliderObject = p_colliderObj;
//	m_spikeDamageRequirement.debugObject = p_debugObj;
//}
//
//void SpikeSkillSystem::ActivateSkillOne()
//{
//
//}
