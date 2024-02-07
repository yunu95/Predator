#include "InWanderLand.h"
#include "WarriorProduction.h"
#include "BleedingComponent.h"
#include "KnockBackComponent.h"
#include "MeleeAttackSystem.h"
#include "WarriorSkillSystem.h"
#include "OnlyDamageComponent.h"
#include "DebugMeshes.h"
#include "HealerProduction.h"
#include "MagicianProduction.h"

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

#pragma region Auto Attack Setting (Including Passive Logic)
	auto unitAttackColliderObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	unitAttackColliderObject->setName("WarriorAutoAttackCollider");
	auto m_physicsCollider = unitAttackColliderObject->AddComponent<physics::BoxCollider>();
	m_physicsCollider->SetHalfExtent({ 1.0f * LENGTH_UNIT, 1.0f * LENGTH_UNIT, 3.0f * LENGTH_UNIT });
	unitAttackColliderObject->AddComponent<physics::RigidBody>()->SetAsKinematic(true);
	//unitAttackColliderObject->SetParent(m_unitGameObject);
	unitAttackColliderObject->GetTransform()->SetWorldPosition({ 0.0f, 0.0f, 3.0f * LENGTH_UNIT });

	// warrior Passive Bleeding System
	auto warriorBleedingSystem = unitAttackColliderObject->AddComponent<BleedingComponent>();
	warriorBleedingSystem->SetSkillOwnerUnit(m_unitComponent);

	auto autoAttackDebugMesh = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	AttachDebugMesh(autoAttackDebugMesh, DebugMeshType::Cube, yunuGI::Color::red(), true);
	autoAttackDebugMesh->GetTransform()->SetLocalScale({ 1.0f * LENGTH_UNIT, 1.0f * LENGTH_UNIT, 3.0f * LENGTH_UNIT });

	auto warriorAttackSystem = m_unitGameObject->AddComponent<MeleeAttackSystem>();
	warriorAttackSystem->SetColliderObject(unitAttackColliderObject);
	warriorAttackSystem->SetColliderDebugObject(autoAttackDebugMesh);
	warriorAttackSystem->SetOwnerUnitObject(m_unitGameObject);
	warriorAttackSystem->SetColliderRemainTime(0.8f);
#pragma endregion

#pragma region Q Skill Setting
	m_QSkillRadius = 3.0f * LENGTH_UNIT;

	auto qSkillKnockBackObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();

	auto qSkillColliderComponent = qSkillKnockBackObject->AddComponent<physics::SphereCollider>();
	qSkillColliderComponent->SetRadius(m_QSkillRadius);
	qSkillKnockBackObject->AddComponent<physics::RigidBody>()->SetAsKinematic(true);

	auto qSkillColliderDebugObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	AttachDebugMesh(qSkillColliderDebugObject, DebugMeshType::Sphere, yunuGI::Color::red(), true);
	qSkillColliderDebugObject->GetTransform()->SetLocalScale({ m_QSkillRadius, m_QSkillRadius, m_QSkillRadius });

	auto knockBackComponent = qSkillKnockBackObject->AddComponent<KnockBackComponent>();
	knockBackComponent->SetSkillOwnerUnit(m_unitComponent);
	qSkillKnockBackObject->SetParent(m_unitGameObject);
#pragma endregion

#pragma region W Skill Setting
	m_WSkillRadius = 3.0f * LENGTH_UNIT;

	auto wSkillColliderObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	auto wSkillColliderComponent = wSkillColliderObject->AddComponent<physics::SphereCollider>();
	wSkillColliderComponent->SetRadius(m_WSkillRadius);
	wSkillColliderObject->AddComponent<physics::RigidBody>()->SetAsKinematic(true);
	wSkillColliderObject->SetParent(m_unitGameObject);
	auto wSkillDamageComponent = wSkillColliderObject->AddComponent<OnlyDamageComponent>();
	wSkillDamageComponent->SetSkillOwnerUnit(m_unitComponent);
	wSkillDamageComponent->SetSkillDamage(10.0f);

	auto wSkillColliderDebugObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	AttachDebugMesh(wSkillColliderDebugObject, DebugMeshType::Sphere, yunuGI::Color::green(), true);
	wSkillColliderDebugObject->GetTransform()->SetLocalScale({ m_WSkillRadius, m_WSkillRadius, m_WSkillRadius });

	// warrior SkillSystem
	auto warriorSkillSystem = m_unitGameObject->AddComponent<WarriorSkillSystem>();
	warriorSkillSystem->SetQSkillKnockBackObject(qSkillKnockBackObject);
	warriorSkillSystem->SetWSkillObject(wSkillColliderObject);
	warriorSkillSystem->SetKnockBackDebugObject(qSkillColliderDebugObject, m_QSkillRadius);
	warriorSkillSystem->SetWSkillDebugObject(wSkillColliderDebugObject, m_WSkillRadius);
#pragma endregion

}

GameObject* WarriorProduction::CreateUnitWithOrder()
{
	auto unitGameObject = UnitProductionOrder::CreateUnitWithOrder();
	return unitGameObject;
}

// 전략 패턴을 설명하기 위한 예시 코드
// 전략 패턴의 핵심은 객체의 동작을 전략 객체를 통해 바꿀 수 있다는 것.
// 아래의 경우 UnitProductionOrder가 전략에 해당되며, 이 전략 객체가 바뀜에 따라 MakeSome 동작이 바뀐다.
class Someone
{
	// 아래가 전략!
	UnitProductionOrder* currentStrategy;
	void MakeSome()
	{
		currentStrategy->CreateUnitWithOrder();
	}
	void ChangeStrategy(UnitProductionOrder* newStrategy)
	{
		currentStrategy = newStrategy;
	}
	static void SampleCode()
	{
		Someone someDude;
		someDude.ChangeStrategy(new WarriorProduction);
		someDude.MakeSome();
		someDude.ChangeStrategy(new MagicianProduction);
		someDude.MakeSome();
		someDude.ChangeStrategy(new HealerProduction);
		someDude.MakeSome();
	}
};