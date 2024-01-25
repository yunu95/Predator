#include "InWanderLand.h"
#include "HealerProduction.h"
#include "RangedAttackSystem.h"
#include "OnlyDamageComponent.h"
#include "DebugMeshes.h"
#include "HealerSkillSystem.h"

void HealerProduction::SetUnitData(GameObject* fbxObject, NavigationField* navField, Vector3d startPosition)
{
#pragma region Unit Status Member Setting
	m_objectName = "Healer";
	m_unitType = Unit::UnitType::Healer;
	m_unitSide = Unit::UnitSide::Player;

	m_healthPoint = 120;
	m_manaPoint = 100;

	m_autoAttackDamage = 8;
	m_criticalHitProbability = 0.1f;
	m_criticalHitMultiplier = 1.5f;

	m_defensePoint = 5;
	m_dodgeProbability = 0.05f;
	m_criticalDamageDecreaseMultiplier = 0.05f;

	m_idRadius = 10.0f;
	m_atkRadius = 8.5f;
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
			m_baseUnitAnimations.m_attackAnimation->SetLoop(true);
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
	auto QSkillFieldObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	auto fieldDamageComponent = QSkillFieldObject->AddComponent<OnlyDamageComponent>();
	fieldDamageComponent->SetSkillOwnerUnit(m_unitComponent);

	auto QSkillFieldCollider = QSkillFieldObject->AddComponent<physics::SphereCollider>();
	m_QSkillFieldRadius = 2.0f;
	QSkillFieldCollider->SetRadius(m_QSkillFieldRadius);
	QSkillFieldObject->AddComponent<physics::RigidBody>()->SetAsKinematic(true);
	QSkillFieldObject->SetParent(m_unitGameObject);

	auto QSkillFieldDebugObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	AttachDebugMesh(QSkillFieldDebugObject, DebugMeshType::Sphere, yunuGI::Color::white(), false);
#pragma endregion

#pragma region SkillSystem Setting
	auto healerSkillSystem = m_unitGameObject->AddComponent<HealerSkillSystem>();
	healerSkillSystem->SetQSkillCollider(QSkillFieldCollider);
	healerSkillSystem->SetQSkillObject(QSkillFieldObject);
	healerSkillSystem->SetQSkillDebugInfo(QSkillFieldDebugObject, m_QSkillFieldRadius);


#pragma endregion
}

yunutyEngine::GameObject* HealerProduction::CreateUnitWithOrder()
{
	auto unitGameObject = UnitProductionOrder::CreateUnitWithOrder();
	return unitGameObject;
}
