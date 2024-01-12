#include "InWanderLand.h"
#include "MeleeEnemyProduction.h"
#include "MeleeAttackSystem.h"
#include "UnitTransformComponent.h"
#include "DebugMeshes.h"

void MeleeEnemyProduction::SetUnitData(GameObject* fbxObject, NavigationField* navField, Vector3d startPosition)
{
	m_objectName = "MeleeEnemy";
	m_unitType = Unit::UnitType::MeleeEnemy;
	m_unitSide = Unit::UnitSide::Enemy;
	m_hp = 100000;
	m_ap = 10;
	m_idRadius = 2.0f;
	m_atkRadius = 1.5f;
	m_unitSpeed = 1.5f;
	m_navField = navField;
	m_startPosition = startPosition;
	m_attackDelay = 2.0f;

	auto rsrcManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();

	/// Setting Animation Related Members
	m_unitGameObject = fbxObject;

	auto animator = m_unitGameObject->GetComponent<yunutyEngine::graphics::Animator>();

	auto& animList = rsrcManager->GetAnimationList();

	for (auto each : animList)
	{
		if (each->GetName() == L"root|000.Idle")
		{
			m_idleAnimation = each;
			m_idleAnimation->SetLoop(true);
			animator->GetGI().PushAnimation(m_idleAnimation);
			animator->GetGI().Play(m_idleAnimation);
		}
		else if (each->GetName() == L"root|001-2.Walk")
		{
			m_walkAnimation = each;
			m_walkAnimation->SetLoop(true);
			animator->GetGI().PushAnimation(m_walkAnimation);
		}
		else if (each->GetName() == L"root|003-1.NormalAttack_L")
		{
			m_attackAnimation = each;
			m_attackAnimation->SetLoop(false);
			animator->GetGI().PushAnimation(m_attackAnimation);
		}
		else if (each->GetName() == L"root|012.Death")
		{
			m_deathAnimation = each;
			m_deathAnimation->SetLoop(false);
			animator->GetGI().PushAnimation(m_deathAnimation);
		}
	}


	/// 임시 - UnitTransformComponent 생성
	m_unitTransformGameObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	auto unitTransformComponent = m_unitTransformGameObject->AddComponent<UnitTransformComponent>();
	unitTransformComponent->ownerObject = m_unitGameObject;

	auto unitAttackColliderObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	unitAttackColliderObject->setName("UnitAttackCollider");

	auto m_physicsCollider = unitAttackColliderObject->AddComponent<physics::BoxCollider>();
	m_physicsCollider->SetHalfExtent({ 0.5,0.5,0.5 });

	auto debugColliderMesh = AttachDebugMesh(unitAttackColliderObject, DebugMeshType::Rectangle, yunuGI::Color::red(), true);

	//m_unitAttackColliderObject->SetParent(m_unitGameObject);
	//m_unitAttackColliderObject->GetTransform()->SetWorldPosition({ 0.0f, 0.0f, 1.3f });

	auto warriorAttackSystem = m_unitGameObject->AddComponent<MeleeAttackSystem>();
	warriorAttackSystem->SetColliderObject(unitAttackColliderObject);
	warriorAttackSystem->SetColliderRemainTime(1.0f);

	unitAttackColliderObject->SetParent(m_unitTransformGameObject);
	unitAttackColliderObject->GetTransform()->SetWorldPosition({ 0.0f, 0.0f, -2.0f });
	//m_unitAttackColliderObject->SetSelfActive(false);
}

yunutyEngine::GameObject* MeleeEnemyProduction::CreateUnitWithOrder()
{
	auto unitGameObject = UnitProductionOrder::CreateUnitWithOrder();
	return unitGameObject;
}
