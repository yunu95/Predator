#include "WarriorProduction.h"
#include "BleedingComponent.h"
#include "KnockBackComponent.h"
#include "MeleeAttackSystem.h"
#include "WarriorSkillSystem.h"
#include "UnitTransformComponent.h"
#include "OnlyDamageComponent.h"
#include "DebugMeshes.h"

void WarriorProduction::SetUnitData(GameObject* fbxObject, NavigationField* navField, Vector3d startPosition)
{
	m_objectName = "Warrior";
	m_unitType = Unit::UnitType::Warrior;
	m_unitSide = Unit::UnitSide::Player;
	m_hp = 1050;
	m_ap = 10;
	m_idRadius = 2.0f;
	m_atkRadius = 1.5f;
	m_unitSpeed = 1.5f;
	m_navField = navField;
	m_startPosition = startPosition;
	m_attackDelay = 1.0f;

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

	/// Choose AttackType

	/// 임시 - UnitTransformComponent 생성
	m_unitTransformGameObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	auto unitTransformComponent = m_unitTransformGameObject->AddComponent<UnitTransformComponent>();
	unitTransformComponent->ownerObject = m_unitGameObject;

	/// Unit Attack Collider Setting
	auto unitAttackColliderObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	unitAttackColliderObject->setName("WarriorAutoAttackCollider");

	auto m_physicsCollider = unitAttackColliderObject->AddComponent<physics::BoxCollider>();
	m_physicsCollider->SetHalfExtent({ 1.0f, 1.0f, 3.0f });
	
	// warrior Passive Bleeding System
	auto warriorBleedingSystem = unitAttackColliderObject->AddComponent<BleedingComponent>();
	auto debugColliderMesh = AttachDebugMesh(unitAttackColliderObject, DebugMeshType::Rectangle, yunuGI::Color::red(), true);
	unitAttackColliderObject->AddComponent<physics::RigidBody>()->SetAsKinematic(true);

	auto warriorAttackSystem = m_unitGameObject->AddComponent<MeleeAttackSystem>();
	warriorAttackSystem->SetColliderObject(unitAttackColliderObject);
	warriorAttackSystem->SetColliderRemainTime(0.05f);
	//m_unitAttackColliderObject->SetSelfActive(false);

	unitAttackColliderObject->SetParent(m_unitTransformGameObject);
	unitAttackColliderObject->GetTransform()->SetWorldPosition({ 0.0f, 0.0f, -2.0f });

	// warrior QSkill KnockBack Object Setting
	auto qSkillKnockBackObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	qSkillKnockBackObject->AddComponent<physics::SphereCollider>()->SetRadius(3.0f);
	qSkillKnockBackObject->AddComponent<physics::RigidBody>()->SetAsKinematic(true);
	qSkillKnockBackObject->SetParent(m_unitTransformGameObject);
	qSkillKnockBackObject->AddComponent<KnockBackComponent>();
	qSkillKnockBackObject->SetParent(m_unitTransformGameObject);
	qSkillKnockBackObject->GetTransform()->SetWorldPosition({ 0.0f, 0.0f, 0.0f });

	// warrior WSkill Damage Collider Object Setting
	auto wSkillColliderObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	auto wSkillColliderComponent = wSkillColliderObject->AddComponent<physics::SphereCollider>();
	wSkillColliderComponent->SetRadius(3.0f);
	wSkillColliderObject->AddComponent<physics::RigidBody>()->SetAsKinematic(true);
	wSkillColliderObject->SetParent(m_unitTransformGameObject);
	auto wSkillDamageComponent = wSkillColliderObject->AddComponent<OnlyDamageComponent>();
	wSkillDamageComponent->SetSkillDamage(1000.0f);

	// warrior SkillSystem
	auto warriorSkillSystem = m_unitGameObject->AddComponent<WarriorSkillSystem>();
	warriorSkillSystem->SetQSkillKnockBackCollider(qSkillKnockBackObject->GetComponent<physics::SphereCollider>());
	warriorSkillSystem->SetWSkillCollider(wSkillColliderComponent);
	warriorSkillSystem->SetUnitTransformObject(m_unitTransformGameObject);


}

GameObject* WarriorProduction::CreateUnitWithOrder()
{
	auto unitGameObject = UnitProductionOrder::CreateUnitWithOrder();
	return unitGameObject;
}

