#include "RangedEnemyProductor.h"
#include "SingleNavigationField.h"
#include "RangedAttackSystem.h"
#include "DebugMeshes.h"
#include "Unit_TemplateData.h"
#include "BossSkillSystem.h"
#include "BurnEffect.h"

void RangedEnemyProductor::SetUnitData()
{
	m_objectName = "MeleeEnenmy";
	m_unitType = Unit::UnitType::RangedEnemy;
	m_unitSide = Unit::UnitSide::Enemy;

	m_healthPoint = 10;
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

	m_unitFbxName = "SKM_Monster2";
}

void RangedEnemyProductor::SingletonInitializer()
{
	SetUnitData();
}

Unit* RangedEnemyProductor::CreateUnit(Vector3d startPos)
{
#pragma region Animation Related Member Setting
	m_unitGameObject = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("SKM_Monster2");
	m_unitGameObject->GetTransform()->SetWorldPosition(startPos);

	/// UnitComponent 추가
	m_unitComponent = m_unitGameObject->AddComponent<Unit>();
	UnitProductor::SetUnitComponentMembers();

#pragma endregion


#pragma region Auto Attack Setting
	auto rangedAttackSystem = m_unitGameObject->AddComponent<RangedAttackSystem>();
	rangedAttackSystem->SetBulletSpeed(10.0f);
#pragma endregion

	if (isEliteMonster)
	{
#pragma region Boss Skill_2 Object Setting
		auto skillTwoColliderObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
		auto skillTwoCollider = skillTwoColliderObject->AddComponent<physics::BoxCollider>();
		float skillTwoColliderRange = 6.0f * UNIT_LENGTH;
		float skillTwoColliderLength = 2.0f * UNIT_LENGTH;
		skillTwoCollider->SetHalfExtent({ skillTwoColliderLength * 0.5f,skillTwoColliderLength * 0.5f, skillTwoColliderRange * 0.5f });

		auto skillTwoDebugMesh = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
		AttachDebugMesh(skillTwoDebugMesh, DebugMeshType::Cube, yunuGI::Color::red(), true);
		skillTwoDebugMesh->GetTransform()->SetWorldScale({ skillTwoColliderLength ,skillTwoColliderLength ,skillTwoColliderRange });
#pragma endregion

		auto eliteSkillSystem = m_unitGameObject->AddComponent<BossSkillSystem>();
		eliteSkillSystem->SelectSkill(Unit::SkillEnum::BossSkillTwo);
		skillTwoColliderObject->SetParent(m_unitGameObject);
		skillTwoDebugMesh->SetParent(m_unitGameObject);
		eliteSkillSystem->SetSkillTwoRange(skillTwoColliderRange);
		eliteSkillSystem->SetSkillTwoRequirments(skillTwoColliderObject, skillTwoDebugMesh);

		auto skinnedMeshRenderer = m_unitGameObject->GetChildren()[0]->GetComponent<yunutyEngine::graphics::SkinnedMesh>();
		auto material = skinnedMeshRenderer->GetGI().GetMaterial();
		auto clonedMaterial = graphics::Renderer::SingleInstance().GetResourceManager()->CloneMaterial(L"Red", material);
		clonedMaterial->SetColor(yunuGI::Color::red());
		skinnedMeshRenderer->GetGI().SetMaterial(0, clonedMaterial);
	}

	UnitProductor::AddRangeSystemComponent();
	UnitProductor::AddColliderComponent();
	UnitProductor::AddNavigationComponent();
	UnitProductor::AddDotweenComponent();

	auto rsrcManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
	auto animator = m_unitGameObject->GetComponent<yunutyEngine::graphics::Animator>();
	auto& animList = rsrcManager->GetAnimationList();
	for (auto each : animList)
	{
		if (each->GetName() == L"Ani_Monster2_Idle")
		{
			m_baseUnitAnimations.m_idleAnimation = each;
			m_baseUnitAnimations.m_idleAnimation->SetLoop(true);
			animator->PushAnimation(m_baseUnitAnimations.m_idleAnimation);
			animator->Play(m_baseUnitAnimations.m_idleAnimation);
		}
		else if (each->GetName() == L"Ani_Monster2_Walk")
		{
			m_baseUnitAnimations.m_walkAnimation = each;
			m_baseUnitAnimations.m_walkAnimation->SetLoop(true);
			animator->PushAnimation(m_baseUnitAnimations.m_walkAnimation);
		}
		else if (each->GetName() == L"Ani_Monster2_Attack")
		{
			m_baseUnitAnimations.m_attackAnimation = each;
			m_baseUnitAnimations.m_attackAnimation->SetLoop(false);
		}
		else if (each->GetName() == L"Ani_Monster2_Skill")
		{
			m_baseUnitAnimations.m_paralysisAnimation = each;
			m_baseUnitAnimations.m_paralysisAnimation->SetLoop(false);
			animator->PushAnimation(m_baseUnitAnimations.m_paralysisAnimation);
		}
		if (each->GetName() == L"Ani_Monster2_Skill")
		{
			m_baseUnitAnimations.m_deathAnimation = each;
			m_baseUnitAnimations.m_deathAnimation->SetLoop(false);
			animator->PushAnimation(m_baseUnitAnimations.m_deathAnimation);
		}
		/// Skill Animation
		if (each->GetName() == L"Ani_Monster2_Skill")
		{
			each->SetLoop(false);
			animator->PushAnimation(each);
			m_unitComponent->RegisterSkillAnimation(Unit::SkillEnum::BossSkillOne, each);
		}
	}
	m_unitComponent->unitAnimations = m_baseUnitAnimations;

	return m_unitComponent;
}

void RangedEnemyProductor::MappingUnitData(application::editor::POD_Unit_TemplateData p_podData)
{
	UnitProductor::MappingUnitData(p_podData);

	isEliteMonster = p_podData.isEliteMonster;
}
