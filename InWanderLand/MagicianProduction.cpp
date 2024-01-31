#include "InWanderLand.h"
#include "MagicianProduction.h"
#include "RangedAttackSystem.h"
#include "MagicianSkillSystem.h"
#include "ParalysisFieldComponent.h"
#include "BlindFieldComponent.h"
#include "DebugMeshes.h"

void MagicianProduction::SetUnitData(GameObject* fbxObject, NavigationField* navField, Vector3d startPosition)
{
#pragma region Unit Status Member Setting
	m_objectName = "Magician";
	m_unitType = Unit::UnitType::Magician;
	m_unitSide = Unit::UnitSide::Player;

	m_healthPoint = 150;
	m_manaPoint = 100;

	m_autoAttackDamage = 15;
	m_criticalHitProbability = 0.5f;
	m_criticalHitMultiplier = 1.5f;

	m_defensePoint = 5;
	m_dodgeProbability = 0.05f;
	m_criticalDamageDecreaseMultiplier = 0.05f;

	m_idRadius = 10.0f * LENGTH_UNIT;
	m_atkRadius = 3.5f * LENGTH_UNIT;
	m_unitSpeed = 4.5f;

	m_attackDelay = 5.0f;

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
	auto magicianAttackSystem = m_unitGameObject->AddComponent<RangedAttackSystem>();
	magicianAttackSystem->SetBulletSpeed(10.0f);
#pragma endregion

#pragma region Q Skill Setting
	auto QSkillProjectileObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	
	QSkillProjectileObject->AddComponent<Dotween>();

	auto QSkillProjectileCollider = QSkillProjectileObject->AddComponent<physics::SphereCollider>();
	m_QSkillProjectileRadius = 1.0f * LENGTH_UNIT;
	QSkillProjectileCollider->SetRadius(m_QSkillProjectileRadius);
	QSkillProjectileObject->AddComponent<physics::RigidBody>()->SetAsKinematic(true);

	auto QSkillProjectileDebugObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	AttachDebugMesh(QSkillProjectileDebugObject, DebugMeshType::Sphere, yunuGI::Color::red(), false);
	QSkillProjectileDebugObject->GetTransform()->scale = { m_QSkillProjectileRadius , m_QSkillProjectileRadius , m_QSkillProjectileRadius };

	auto QSkillFieldObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	QSkillFieldObject->AddComponent<Dotween>();
	auto fieldDamageComponent = QSkillFieldObject->AddComponent<BlindFieldComponent>();
	fieldDamageComponent->SetSkillOwnerUnit(m_unitComponent);
	auto QSkillFieldCollider = QSkillFieldObject->AddComponent<physics::SphereCollider>();
	m_QSkillFieldRadius = 2.0f * LENGTH_UNIT;
	QSkillFieldCollider->SetRadius(m_QSkillFieldRadius);
	QSkillFieldObject->AddComponent<physics::RigidBody>()->SetAsKinematic(true);
	auto QSkillFieldDebugObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	AttachDebugMesh(QSkillFieldDebugObject, DebugMeshType::Sphere, yunuGI::Color::white(), false);
	QSkillFieldDebugObject->GetTransform()->scale = { m_QSkillFieldRadius , m_QSkillFieldRadius , m_QSkillFieldRadius };
#pragma endregion

#pragma region W Skill Setting
	auto WSkillProjectileObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	WSkillProjectileObject->AddComponent<Dotween>();

	auto WSkillProjectileCollider = WSkillProjectileObject->AddComponent<physics::SphereCollider>();
	m_WSkillProjectileRadius = 1.0f * LENGTH_UNIT;
	WSkillProjectileCollider->SetRadius(m_WSkillProjectileRadius);
	WSkillProjectileObject->AddComponent<physics::RigidBody>()->SetAsKinematic(true);

	auto WSkillProjectileDebugObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	AttachDebugMesh(WSkillProjectileDebugObject, DebugMeshType::Sphere, yunuGI::Color::green(), false);
	WSkillProjectileDebugObject->GetTransform()->scale = { m_WSkillProjectileRadius , m_WSkillProjectileRadius , m_WSkillProjectileRadius };

	auto WSkillFieldObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	WSkillFieldObject->AddComponent<Dotween>();
	auto WfieldDamageComponent = WSkillFieldObject->AddComponent<ParalysisFieldComponent>();
	WfieldDamageComponent->SetSkillOwnerUnit(m_unitComponent);
	auto WSkillFieldCollider = WSkillFieldObject->AddComponent<physics::SphereCollider>();
	m_WSkillFieldRadius = 2.0f * LENGTH_UNIT;
	WSkillFieldCollider->SetRadius(m_WSkillFieldRadius);
	WSkillFieldObject->AddComponent<physics::RigidBody>()->SetAsKinematic(true);
	auto WSkillFieldDebugObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	AttachDebugMesh(WSkillFieldDebugObject, DebugMeshType::Sphere, yunuGI::Color::blue(), true);
	WSkillFieldDebugObject->GetTransform()->scale = { m_WSkillFieldRadius , m_WSkillFieldRadius , m_WSkillFieldRadius };
#pragma endregion

#pragma endregion

	auto magicianSkillSystem = m_unitGameObject->AddComponent<MagicianSkillSystem>();

	magicianSkillSystem->SetQSkillCollider(QSkillProjectileCollider, QSkillFieldCollider);
	magicianSkillSystem->SetQSkillDebugPair({QSkillProjectileDebugObject, m_QSkillProjectileRadius}, {QSkillFieldDebugObject, m_QSkillFieldRadius});
	magicianSkillSystem->SetQSkillObject(QSkillProjectileObject, QSkillFieldObject);

	magicianSkillSystem->SetWSkillCollider(WSkillProjectileCollider, WSkillFieldCollider);
	magicianSkillSystem->SetWSkillDebugPair({ WSkillProjectileDebugObject, m_WSkillProjectileRadius }, { WSkillFieldDebugObject, m_WSkillFieldRadius });
	magicianSkillSystem->SetWSkillObject(WSkillProjectileObject, WSkillFieldObject);

}

yunutyEngine::GameObject* MagicianProduction::CreateUnitWithOrder()
{
	auto unitGameObject = UnitProductionOrder::CreateUnitWithOrder();
	return unitGameObject;
}
