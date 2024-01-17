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
#pragma region Unit Status Member Setting
	m_objectName = "Warrior";
	m_unitType = Unit::UnitType::Warrior;
	m_unitSide = Unit::UnitSide::Player;

	m_healthPoint = 250;
	m_manaPoint = 100;

	m_autoAttackDamage = 10;
	m_criticalHitProbability = 0.2f;
	m_criticalHitMultiplier = 1.5f;

	m_defensePoint = 15;
	m_dodgeProbability = 0.2f;
	m_criticalDamageDecreaseMultiplier = 0.2f;

	m_idRadius = 2.0f;
	m_atkRadius = 1.5f;
	m_unitSpeed = 1.5f;

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

#pragma region Auto Attack Setting (Including Passive Logic)
	/// 임시 - UnitTransformComponent 생성
	m_unitTransformGameObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	auto unitTransformComponent = m_unitTransformGameObject->AddComponent<UnitTransformComponent>();
	unitTransformComponent->ownerObject = m_unitGameObject;

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
#pragma endregion

#pragma region Q Skill Setting
	m_QSkillRadius = 3.0f;

	auto qSkillKnockBackObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	qSkillKnockBackObject->AddComponent<physics::SphereCollider>()->SetRadius(m_QSkillRadius);
	qSkillKnockBackObject->AddComponent<physics::RigidBody>()->SetAsKinematic(true);
	qSkillKnockBackObject->SetParent(m_unitTransformGameObject);

	auto knockBackComponent = qSkillKnockBackObject->AddComponent<KnockBackComponent>();
	qSkillKnockBackObject->SetParent(m_unitTransformGameObject);
	qSkillKnockBackObject->GetTransform()->SetWorldPosition({ 0.0f, 0.0f, 0.0f });

	auto qSkillColliderDebugObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	auto qSkillDebugMesh = AttachDebugMesh(qSkillColliderDebugObject, DebugMeshType::Sphere, yunuGI::Color::red(), true);
#pragma endregion

#pragma region W Skill Setting
	m_WSkillRadius = 8.0f;

	auto wSkillColliderObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	auto wSkillColliderComponent = wSkillColliderObject->AddComponent<physics::SphereCollider>();
	wSkillColliderComponent->SetRadius(m_WSkillRadius);
	wSkillColliderObject->AddComponent<physics::RigidBody>()->SetAsKinematic(true);
	wSkillColliderObject->SetParent(m_unitTransformGameObject);
	auto wSkillDamageComponent = wSkillColliderObject->AddComponent<OnlyDamageComponent>();
	wSkillDamageComponent->SetSkillDamage(1000.0f);

	auto wSkillColliderDebugObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	auto wSkillDebugMesh = AttachDebugMesh(wSkillColliderDebugObject, DebugMeshType::Sphere, yunuGI::Color::red(), true);

	// warrior SkillSystem
	auto warriorSkillSystem = m_unitGameObject->AddComponent<WarriorSkillSystem>();
	warriorSkillSystem->SetQSkillKnockBackCollider(qSkillKnockBackObject->GetComponent<physics::SphereCollider>());
	warriorSkillSystem->SetWSkillCollider(wSkillColliderComponent);
	warriorSkillSystem->SetUnitTransformObject(m_unitTransformGameObject);
	warriorSkillSystem->SetKnockBackDebugObject(qSkillColliderDebugObject, m_QSkillRadius);
	warriorSkillSystem->SetWSkillDebugObject(wSkillColliderDebugObject, m_WSkillRadius);
#pragma endregion

}

GameObject* WarriorProduction::CreateUnitWithOrder()
{
	auto unitGameObject = UnitProductionOrder::CreateUnitWithOrder();
	return unitGameObject;
}

