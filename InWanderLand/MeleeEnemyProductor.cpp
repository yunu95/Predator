#include "InWanderLand.h"
#include "MeleeEnemyProductor.h"
#include "MeleeAttackSystem.h"
#include "DebugMeshes.h"
#include "SingleNavigationField.h"
#include "Unit_TemplateData.h"
#include "BossSkillSystem.h"
#include "DamageOnlyComponent.h"
#include "BurnEffect.h"

void MeleeEnemyProductor::SetUnitData()
{
	m_objectName = "MeleeEnenmy";
	m_unitType = Unit::UnitType::MeleeEnemy;
	m_unitSide = Unit::UnitSide::Enemy;

	m_healthPoint = 50;
	m_manaPoint = 100;

	m_autoAttackDamage = 10;
	m_criticalHitProbability = 0.2f;
	m_criticalHitMultiplier = 1.5f;

	m_defensePoint = 15;
	m_dodgeProbability = 0.2f;
	m_criticalDamageDecreaseMultiplier = 0.2f;

	m_maxAggroNumber = 10;

	m_idRadius = 4.0f * UNIT_LENGTH;
	m_atkRadius = 1.7f * UNIT_LENGTH;
	m_unitSpeed = 4.5f;

	m_attackDelay = 1.0f;

	m_navField = &SingleNavigationField::Instance();
	m_unitFbxName = "SKM_Monster1";
}

void MeleeEnemyProductor::SingletonInitializer()
{
	SetUnitData();
}

Unit* MeleeEnemyProductor::CreateUnit(Vector3d startPos)
{
#pragma region Animation Related Member Setting
	m_unitGameObject = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("SKM_Monster1");
	m_unitGameObject->GetTransform()->SetWorldPosition(startPos);
	/// UnitComponent 추가
	m_unitComponent = m_unitGameObject->AddComponent<Unit>();
	UnitProductor::SetUnitComponentMembers();

#pragma endregion

#pragma region Auto Attack Setting
	auto meleeAttackSystem = m_unitGameObject->AddComponent<MeleeAttackSystem>();
	meleeAttackSystem->SetMeleeAttackType(MeleeAttackType::DirectAttack);
	//meleeAttackSystem->SetColliderObject(unitAttackColliderObject);
	//meleeAttackSystem->SetColliderDebugObject(autoAttackDebugMesh);
	meleeAttackSystem->SetOwnerUnitObject(m_unitGameObject);
	//meleeAttackSystem->SetColliderRemainTime(0.3f);
	meleeAttackSystem->SetDamage(m_autoAttackDamage);
#pragma endregion

#pragma region Elite Skill Object Setting
	if (isEliteMonster)
	{
		auto skillOneColliderObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
		auto skillOneCollider = skillOneColliderObject->AddComponent<physics::SphereCollider>();
		float skillOneRadius = 5.0f * UNIT_LENGTH;
		skillOneCollider->SetRadius(skillOneRadius);
		skillOneColliderObject->AddComponent<physics::RigidBody>()->SetAsKinematic(true);
		auto skillDamageComponent = skillOneColliderObject->AddComponent<DamageOnlyComponent>();
		skillDamageComponent->SetSkillOwnerUnit(m_unitComponent);
		skillDamageComponent->SetSkillDamage(3.0f);

		auto skillOneDebugMesh = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
		AttachDebugMesh(skillOneDebugMesh, DebugMeshType::Sphere, yunuGI::Color::red(), true);
		skillOneDebugMesh->GetTransform()->SetWorldScale({ skillOneRadius * 2 ,skillOneRadius * 2 ,skillOneRadius * 2 });

		auto eliteSkillSystem = m_unitGameObject->AddComponent<BossSkillSystem>();
		eliteSkillSystem->SelectSkill(Unit::SkillEnum::BossSkillOne);
		skillOneColliderObject->SetParent(m_unitGameObject);
		skillOneDebugMesh->SetParent(m_unitGameObject);
		eliteSkillSystem->SetSkillOneRequirments(skillOneColliderObject, skillOneDebugMesh);

		auto skinnedMeshRenderer = m_unitGameObject->GetChildren()[0]->GetComponent<yunutyEngine::graphics::SkinnedMesh>();
		auto material = skinnedMeshRenderer->GetGI().GetMaterial();
		auto clonedMaterial = graphics::Renderer::SingleInstance().GetResourceManager()->CloneMaterial(L"Red", material);
		clonedMaterial->SetColor(yunuGI::Color::red());
		skinnedMeshRenderer->GetGI().SetMaterial(0, clonedMaterial);
	}
#pragma endregion

	UnitProductor::AddRangeSystemComponent();
	UnitProductor::AddColliderComponent();
	UnitProductor::AddNavigationComponent();
	UnitProductor::AddDotweenComponent();

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
		}
		if (each->GetName() == L"Ani_Monster1_Skill")
		{
			m_baseUnitAnimations.m_paralysisAnimation = each;
			m_baseUnitAnimations.m_paralysisAnimation->SetLoop(false);
			animator->PushAnimation(m_baseUnitAnimations.m_paralysisAnimation);
		}
		if (each->GetName() == L"Ani_Monster1_Skill")
		{
			m_baseUnitAnimations.m_deathAnimation = each;
			m_baseUnitAnimations.m_deathAnimation->SetLoop(false);
			animator->PushAnimation(m_baseUnitAnimations.m_deathAnimation);
		}
		/// Skill Animation
		if (each->GetName() == L"Ani_Monster1_Skill")
		{
			each->SetLoop(false);
			animator->PushAnimation(each);
			m_baseUnitAnimations.m_skillOneAnimation = each;
			m_unitComponent->RegisterSkillAnimation(Unit::SkillEnum::BossSkillOne, each);
		}
	}
	m_unitComponent->unitAnimations = m_baseUnitAnimations;
	SetUnitAnimationFunction();
	return m_unitComponent;
}

void MeleeEnemyProductor::MappingUnitData(application::editor::POD_Unit_TemplateData p_podData)
{
	UnitProductor::MappingUnitData(p_podData);

	isEliteMonster = p_podData.isEliteMonster;
}
