#include "TriggerTrapProductor.h"
#include "SingleNavigationField.h"
#include "ChessTrapSkillSystem.h"
#include "DebugMeshes.h"
#include "DamageOnlyComponent.h"
#include "TrapTriggerSensor.h"

void TriggerTrapProductor::SetUnitData()
{
	m_objectName = "TriggeredTrap";
	m_unitType = Unit::UnitType::ChessTrap;
	m_unitSide = Unit::UnitSide::Enemy;

	m_healthPoint = 1;
	m_manaPoint = 100;

	m_autoAttackDamage = 15;
	m_criticalHitProbability = 0.5f;
	m_criticalHitMultiplier = 1.5f;

	m_defensePoint = 5;
	m_dodgeProbability = 0.05f;
	m_criticalDamageDecreaseMultiplier = 0.05f;

	m_maxAggroNumber = 3;

	m_idRadius = 0.0f * UNIT_LENGTH;
	m_atkRadius = 0.0f * UNIT_LENGTH;
	m_unitSpeed = 4.5f;

	m_attackDelay = 5.0f;

	m_navField = &SingleNavigationField::Instance();

	m_unitFbxName = "SKM_Monster1";
}

void TriggerTrapProductor::SingletonInitializer()
{
	SetUnitData();
}

Unit* TriggerTrapProductor::CreateUnit(Vector3d startPos)
{
#pragma region Animation Related Member Setting
	m_unitGameObject = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("SKM_Monster1");
	m_unitGameObject->GetTransform()->SetWorldPosition(startPos);

	/// UnitComponent 추가
	m_unitComponent = m_unitGameObject->AddComponent<Unit>();
	UnitProductor::SetUnitComponentMembers();




	auto rsrcManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
	auto animator = m_unitGameObject->GetComponent<yunutyEngine::graphics::Animator>();
	auto& animList = rsrcManager->GetAnimationList();
	for (auto each : animList)
	{
		if (each->GetName() == L"Ani_Monster1_Idle")
		{
			m_baseUnitAnimations.m_idleAnimation = each;
			m_baseUnitAnimations.m_idleAnimation->SetLoop(true);
			animator->PushAnimation(m_baseUnitAnimations.m_idleAnimation);
			animator->Play(m_baseUnitAnimations.m_idleAnimation);
		}
		else if (each->GetName() == L"Ani_Monster1_Walk")
		{
			m_baseUnitAnimations.m_walkAnimation = each;
			m_baseUnitAnimations.m_walkAnimation->SetLoop(true);
			animator->PushAnimation(m_baseUnitAnimations.m_walkAnimation);
		}
		else if (each->GetName() == L"Ani_Monster1_Attack")
		{
			m_baseUnitAnimations.m_attackAnimation = each;
			m_baseUnitAnimations.m_attackAnimation->SetLoop(false);
			animator->PushAnimation(m_baseUnitAnimations.m_attackAnimation);
		}
		//if (each->GetName() == L"Ani_Monster1_Skill")
		//{
		//	m_baseUnitAnimations.m_paralysisAnimation = each;
		//	m_baseUnitAnimations.m_paralysisAnimation->SetLoop(false);
		//	animator->PushAnimation(m_baseUnitAnimations.m_paralysisAnimation);
		//}
		//if (each->GetName() == L"Ani_Monster1_Skill")
		//{
		//	m_baseUnitAnimations.m_deathAnimation = each;
		//	m_baseUnitAnimations.m_deathAnimation->SetLoop(false);
		//	animator->PushAnimation(m_baseUnitAnimations.m_deathAnimation);
		//}
		/// Skill Animation
		if (each->GetName() == L"Ani_Monster1_Skill")
		{
			each->SetLoop(false);
			m_unitComponent->RegisterSkillAnimation(Unit::SkillEnum::BossSkillOne, each);
		}
	}
	m_unitComponent->unitAnimations = m_baseUnitAnimations;

	return m_unitComponent;
}
