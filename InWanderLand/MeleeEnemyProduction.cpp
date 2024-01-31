#include "InWanderLand.h"
#include "MeleeEnemyProduction.h"
#include "MeleeAttackSystem.h"
#include "DebugMeshes.h"

void MeleeEnemyProduction::SetUnitData(GameObject* fbxObject, NavigationField* navField, Vector3d startPosition)
{
#pragma region Unit Status Member Setting
	m_objectName = "MeleeEnenmy";
	m_unitType = Unit::UnitType::MeleeEnemy;
	m_unitSide = Unit::UnitSide::Enemy;

	m_healthPoint = 500;
	m_manaPoint = 100;

	m_autoAttackDamage = 10;
	m_criticalHitProbability = 0.2f;
	m_criticalHitMultiplier = 1.5f;

	m_defensePoint = 15;
	m_dodgeProbability = 0.2f;
	m_criticalDamageDecreaseMultiplier = 0.2f;

	m_idRadius = 4.0f * LENGTH_UNIT;
	m_atkRadius = 1.7f * LENGTH_UNIT;
	m_unitSpeed = 4.5f;

	m_attackDelay = 1.0f;

	m_navField = navField;
	m_startPosition = startPosition;
#pragma endregion

#pragma region Animation Related Member Setting
	m_unitGameObject = fbxObject;
	auto rsrcManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
	auto animator = m_unitGameObject->GetComponent<yunutyEngine::graphics::Animator>();
	auto& animList = rsrcManager->GetAnimationList();
	for (auto each : animList)
	{
		if (each->GetName() == L"root|000.Idle")
		{
			m_baseUnitAnimations.m_idleAnimation = each;
			m_baseUnitAnimations.m_idleAnimation->SetLoop(true);
			animator->GetGI().PushAnimation(m_baseUnitAnimations.m_idleAnimation);
			animator->GetGI().Play(m_baseUnitAnimations.m_idleAnimation);
		}
		else if (each->GetName() == L"root|001-2.Walk")
		{
			m_baseUnitAnimations.m_walkAnimation = each;
			m_baseUnitAnimations.m_walkAnimation->SetLoop(true);
			animator->GetGI().PushAnimation(m_baseUnitAnimations.m_walkAnimation);
		}
		else if (each->GetName() == L"root|003-1.NormalAttack_L")
		{
			m_baseUnitAnimations.m_attackAnimation = each;
			m_baseUnitAnimations.m_attackAnimation->SetLoop(false);
			animator->GetGI().PushAnimation(m_baseUnitAnimations.m_attackAnimation);
		}
		else if (each->GetName() == L"root|011-1.Groggy")
		{
			m_baseUnitAnimations.m_paralysisAnimation = each;
			m_baseUnitAnimations.m_paralysisAnimation->SetLoop(false);
			animator->GetGI().PushAnimation(m_baseUnitAnimations.m_paralysisAnimation);
		}
		else if (each->GetName() == L"root|012.Death")
		{
			m_baseUnitAnimations.m_deathAnimation = each;
			m_baseUnitAnimations.m_deathAnimation->SetLoop(false);
			animator->GetGI().PushAnimation(m_baseUnitAnimations.m_deathAnimation);
		}
	}
#pragma endregion

	/// UnitComponent 추가
	m_unitComponent = m_unitGameObject->AddComponent<Unit>();

#pragma region Auto Attack Setting
	auto unitAttackColliderObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	unitAttackColliderObject->setName("UnitAttackCollider");

	auto m_physicsCollider = unitAttackColliderObject->AddComponent<physics::BoxCollider>();
	m_physicsCollider->SetHalfExtent({ 0.5 * LENGTH_UNIT,0.5 * LENGTH_UNIT,0.5 * LENGTH_UNIT });

	auto autoAttackDebugMesh = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	AttachDebugMesh(autoAttackDebugMesh, DebugMeshType::Cube, yunuGI::Color::red(), true);
	autoAttackDebugMesh->GetTransform()->SetLocalScale({ 1.0f * LENGTH_UNIT, 1.0f * LENGTH_UNIT, 3.0f * LENGTH_UNIT });

	auto warriorAttackSystem = m_unitGameObject->AddComponent<MeleeAttackSystem>();
	warriorAttackSystem->SetColliderObject(unitAttackColliderObject);
	warriorAttackSystem->SetColliderDebugObject(autoAttackDebugMesh);
	warriorAttackSystem->SetOwnerUnitObject(m_unitGameObject);
	warriorAttackSystem->SetColliderRemainTime(0.3f);

	unitAttackColliderObject->SetParent(m_unitGameObject);
	unitAttackColliderObject->GetTransform()->SetWorldPosition({ 0.0f, 0.0f, -2.0f });
	autoAttackDebugMesh->SetParent(m_unitGameObject);
	autoAttackDebugMesh->GetTransform()->SetWorldPosition({ 0.0f, 0.0f, -2.0f });
#pragma endregion

}

yunutyEngine::GameObject* MeleeEnemyProduction::CreateUnitWithOrder()
{
	auto unitGameObject = UnitProductionOrder::CreateUnitWithOrder();
	return unitGameObject;
}
