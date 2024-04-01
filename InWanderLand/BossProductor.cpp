#include "BossProductor.h"
#include "DebugMeshes.h"
#include "SingleNavigationField.h"
#include "MeleeAttackSystem.h"

void BossProductor::SetUnitData()
{
	m_objectName = "Boss";
	m_unitType = Unit::UnitType::Boss;
	m_unitSide = Unit::UnitSide::Enemy;

	m_healthPoint = 100000;
	m_manaPoint = 100;

	m_autoAttackDamage = 10;
	m_criticalHitProbability = 0.2f;
	m_criticalHitMultiplier = 1.5f;

	m_defensePoint = 15;
	m_dodgeProbability = 0.2f;
	m_criticalDamageDecreaseMultiplier = 0.2f;

	m_maxAggroNumber = 2;

	m_idRadius = 4.0f * lengthUnit;
	m_atkRadius = 1.7f * lengthUnit;
	m_unitSpeed = 4.5f;

	m_attackDelay = 1.0f;

	m_navField = &SingleNavigationField::Instance();

	m_unitFbxName = "SKM_Robin";
}

void BossProductor::SingletonInitializer()
{
	SetUnitData();
}

Unit* BossProductor::CreateUnit(Vector3d startPos)
{
#pragma region Animation Related Member Setting
	m_unitGameObject = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("SKM_Robin");
	m_unitGameObject->GetTransform()->SetWorldPosition(startPos);

	auto rsrcManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
	auto animator = m_unitGameObject->GetComponent<yunutyEngine::graphics::Animator>();
	auto& animList = rsrcManager->GetAnimationList();
	for (auto each : animList)
	{
		if (each->GetName() == L"Rig_Robin_arpbob|Ani_Robin_Idle")
		{
			m_baseUnitAnimations.m_idleAnimation = each;
			m_baseUnitAnimations.m_idleAnimation->SetLoop(true);
			animator->GetGI().PushAnimation(m_baseUnitAnimations.m_idleAnimation);
			animator->GetGI().Play(m_baseUnitAnimations.m_idleAnimation);
		}
		else if (each->GetName() == L"Rig_Robin_arpbob|Ani_Robin_Walk")
		{
			m_baseUnitAnimations.m_walkAnimation = each;
			m_baseUnitAnimations.m_walkAnimation->SetLoop(true);
			animator->GetGI().PushAnimation(m_baseUnitAnimations.m_walkAnimation);
		}
		else if (each->GetName() == L"Rig_Robin_arpbob|Ani_Robin_BattleStart")
		{
			m_baseUnitAnimations.m_attackAnimation = each;
			m_baseUnitAnimations.m_attackAnimation->SetLoop(false);
			animator->GetGI().PushAnimation(m_baseUnitAnimations.m_attackAnimation);
		}
		else if (each->GetName() == L"Rig_Robin_arpbob|Ani_Robin_BattleMode")
		{
			m_baseUnitAnimations.m_paralysisAnimation = each;
			m_baseUnitAnimations.m_paralysisAnimation->SetLoop(false);
			animator->GetGI().PushAnimation(m_baseUnitAnimations.m_paralysisAnimation);
		}
		else if (each->GetName() == L"Rig_Robin_arpbob|Ani_Robin_APose")
		{
			m_baseUnitAnimations.m_deathAnimation = each;
			m_baseUnitAnimations.m_deathAnimation->SetLoop(false);
			animator->GetGI().PushAnimation(m_baseUnitAnimations.m_deathAnimation);
		}
	}
#pragma endregion
	/// UnitComponent 추가
	m_unitComponent = m_unitGameObject->AddComponent<Unit>();

#pragma region Auto Attack Setting (Including Passive Logic)
	float meleeAttackColliderRange = 3.0f;
	float meleeAttackColliderLength = 1.0f;

	auto unitAttackColliderObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	unitAttackColliderObject->setName("UnitAttackCollider");

	auto m_physicsCollider = unitAttackColliderObject->AddComponent<physics::BoxCollider>();
	m_physicsCollider->SetHalfExtent({ meleeAttackColliderLength * 0.5f * lengthUnit, meleeAttackColliderLength * 0.5f * lengthUnit, meleeAttackColliderRange * 0.5f * lengthUnit });

	auto autoAttackDebugMesh = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	AttachDebugMesh(autoAttackDebugMesh, DebugMeshType::Cube, yunuGI::Color::red(), true);
	autoAttackDebugMesh->GetTransform()->SetLocalScale({ meleeAttackColliderLength * lengthUnit, meleeAttackColliderLength * lengthUnit, meleeAttackColliderRange * lengthUnit });

	auto meleeAttackSystem = m_unitGameObject->AddComponent<MeleeAttackSystem>();
	meleeAttackSystem->SetMeleeAttackType(MeleeAttackType::Collider);
	meleeAttackSystem->SetColliderObject(unitAttackColliderObject);
	meleeAttackSystem->SetColliderDebugObject(autoAttackDebugMesh);
	meleeAttackSystem->SetOwnerUnitObject(m_unitGameObject);
	meleeAttackSystem->SetColliderRemainTime(0.3f);

	//unitAttackColliderObject->SetParent(m_unitGameObject);
	unitAttackColliderObject->GetTransform()->SetWorldPosition({ 0.0f, 0.0f, -1 * meleeAttackColliderRange });
	//autoAttackDebugMesh->SetParent(m_unitGameObject);
	autoAttackDebugMesh->GetTransform()->SetWorldPosition({ 0.0f, 0.0f, -1 * meleeAttackColliderRange });


#pragma endregion

	return m_unitComponent;
}
