#include "BossProductor.h"
#include "DebugMeshes.h"
#include "SingleNavigationField.h"
#include "MeleeAttackSystem.h"
#include "BossSkillSystem.h"

void BossProductor::SetUnitData()
{
	m_objectName = "Boss";
	m_unitType = Unit::UnitType::Boss;
	m_unitSide = Unit::UnitSide::Enemy;

	m_maxHealth = 100000;
	m_manaPoint = 100;

	m_autoAttackDamage = 10;
	m_criticalHitProbability = 0.2f;
	m_criticalHitMultiplier = 1.5f;

	m_defensePoint = 15;
	m_dodgeProbability = 0.2f;
	m_criticalDamageDecreaseMultiplier = 0.2f;

	m_maxAggroNumber = 2;

	m_idRadius = 4.0f * UNIT_LENGTH;
	m_atkRadius = 1.7f * UNIT_LENGTH;
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
	m_unitGameObject = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("SKM_HeartQueen");
	m_unitGameObject->GetTransform()->SetWorldPosition(startPos);

	/// UnitComponent 추가
	m_unitComponent = m_unitGameObject->AddComponent<Unit>();
	UnitProductor::SetUnitComponentMembers();


#pragma endregion

#pragma region Auto Attack Setting (Including Passive Logic)
	float meleeAttackColliderRange = 3.0f;
	float meleeAttackColliderLength = 1.0f;

	auto unitAttackColliderObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	unitAttackColliderObject->setName("UnitAttackCollider");

	auto m_physicsCollider = unitAttackColliderObject->AddComponent<physics::BoxCollider>();
	m_physicsCollider->SetHalfExtent({ meleeAttackColliderLength * 0.5f * UNIT_LENGTH, meleeAttackColliderLength * 0.5f * UNIT_LENGTH, meleeAttackColliderRange * 0.5f * UNIT_LENGTH });

	auto autoAttackDebugMesh = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	AttachDebugMesh(autoAttackDebugMesh, DebugMeshType::Cube, yunuGI::Color::red(), true);
	autoAttackDebugMesh->GetTransform()->SetWorldScale({ meleeAttackColliderLength * UNIT_LENGTH, meleeAttackColliderLength * UNIT_LENGTH, meleeAttackColliderRange * UNIT_LENGTH });

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

#pragma region Boss Skill_1 Object Setting
	auto skillOneColliderObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	auto skillOneCollider = skillOneColliderObject->AddComponent<physics::SphereCollider>();
	float skillOneRadius = 5.0f * UNIT_LENGTH;
	skillOneCollider->SetRadius(skillOneRadius);

	auto skillOneDebugMesh = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	AttachDebugMesh(skillOneDebugMesh, DebugMeshType::Sphere, yunuGI::Color::red(), true);
	skillOneDebugMesh->GetTransform()->SetWorldScale({ skillOneRadius * 2, skillOneRadius * 2, skillOneRadius * 2 });
#pragma endregion

#pragma region Boss Skill_2 Object Setting
	auto skillTwoColliderObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	auto skillTwoCollider = skillTwoColliderObject->AddComponent<physics::BoxCollider>();
	float skillTwoColliderRange = 7.0f * UNIT_LENGTH;
	float skillTwoColliderLength = 3.0f * UNIT_LENGTH;
	skillTwoCollider->SetHalfExtent({ skillTwoColliderLength * 0.5f,skillTwoColliderLength * 0.5f, skillTwoColliderRange * 0.5f });

	auto skillTwoDebugMesh = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	AttachDebugMesh(skillTwoDebugMesh, DebugMeshType::Cube, yunuGI::Color::red(), true);
	skillTwoDebugMesh->GetTransform()->SetWorldScale({ skillTwoColliderLength ,skillTwoColliderLength ,skillTwoColliderRange });
#pragma endregion

#pragma region Boss Skill_4 Object Setting
	auto skillFourColliderObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	auto skillFourCollider = skillFourColliderObject->AddComponent<physics::SphereCollider>();
	float skillFourColliderRadius = 7.0f * UNIT_LENGTH;
	skillFourCollider->SetRadius(skillFourColliderRadius);

	auto skillFourDebugMesh = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	AttachDebugMesh(skillFourDebugMesh, DebugMeshType::Sphere, yunuGI::Color::red(), true);
	skillFourDebugMesh->GetTransform()->SetWorldScale({ skillFourColliderRadius * 2, skillFourColliderRadius * 2, skillFourColliderRadius * 2 });
#pragma endregion

	auto bossSkillSystem = m_unitGameObject->AddComponent<BossSkillSystem>();
	skillOneColliderObject->SetParent(m_unitGameObject);
	skillOneDebugMesh->SetParent(m_unitGameObject);
	skillTwoColliderObject->SetParent(m_unitGameObject);
	skillTwoDebugMesh->SetParent(m_unitGameObject);
	skillFourColliderObject->SetParent(m_unitGameObject);
	skillFourDebugMesh->SetParent(m_unitGameObject);

	bossSkillSystem->SetSkillOneRequirments(skillOneColliderObject, skillOneDebugMesh);
	bossSkillSystem->SetSkillTwoRequirments(skillTwoColliderObject, skillTwoDebugMesh);
	bossSkillSystem->SetSkillTwoRange(skillTwoColliderRange);
	bossSkillSystem->SetSkillFourRequirments(skillFourColliderObject, skillFourCollider, skillFourDebugMesh);

	UnitProductor::AddRangeSystemComponent();
	UnitProductor::AddColliderComponent();
	UnitProductor::AddNavigationComponent();
	UnitProductor::AddDotweenComponent();

	auto rsrcManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
	auto animator = m_unitGameObject->GetComponent<yunutyEngine::graphics::Animator>();
	auto& animList = rsrcManager->GetAnimationList();
	for (auto each : animList)
	{
		if (each->GetName() == L"Rig_Robin|Ani_HeartQueen_Idle")
		{
			m_baseUnitAnimations.m_idleAnimation = each;
			m_baseUnitAnimations.m_idleAnimation->SetLoop(true);
			animator->PushAnimation(m_baseUnitAnimations.m_idleAnimation);
			animator->Play(m_baseUnitAnimations.m_idleAnimation);
		}
		else if (each->GetName() == L"Rig_Robin|Ani_HeartQueen_Walk")
		{
			m_baseUnitAnimations.m_walkAnimation = each;
			m_baseUnitAnimations.m_walkAnimation->SetLoop(true);
			animator->PushAnimation(m_baseUnitAnimations.m_walkAnimation);
		}
		else if (each->GetName() == L"Rig_Robin|Ani_HeartQueen_Attack")
		{
			m_baseUnitAnimations.m_attackAnimation = each;
			m_baseUnitAnimations.m_attackAnimation->SetLoop(false);
			animator->PushAnimation(m_baseUnitAnimations.m_attackAnimation);
		}
		else if (each->GetName() == L"Rig_Robin|Ani_HeartQueen_APose")
		{
			m_baseUnitAnimations.m_paralysisAnimation = each;
			m_baseUnitAnimations.m_paralysisAnimation->SetLoop(false);
			animator->PushAnimation(m_baseUnitAnimations.m_paralysisAnimation);
		}
		else if (each->GetName() == L"Rig_Robin|Ani_HeartQueen_Death")
		{
			m_baseUnitAnimations.m_deathAnimation = each;
			m_baseUnitAnimations.m_deathAnimation->SetLoop(false);
			animator->PushAnimation(m_baseUnitAnimations.m_deathAnimation);
		}
		/// Skill Animation
		if (each->GetName() == L"Rig_Robin|Ani_HeartQueen_Skill1")
		{
			each->SetLoop(true);
			animator->PushAnimation(each);
			m_baseUnitAnimations.m_skillOneAnimation = each;
			m_unitComponent->RegisterSkillAnimation(Unit::SkillEnum::BossSkillOne, each);
		}
		if (each->GetName() == L"Rig_Robin|Ani_HeartQueen_Skill2")
		{
			each->SetLoop(false);
			animator->PushAnimation(each);
			m_baseUnitAnimations.m_skillTwoAnimation = each;
			m_unitComponent->RegisterSkillAnimation(Unit::SkillEnum::BossSkillTwo, each);
		}
		if (each->GetName() == L"Rig_Robin|Ani_HeartQueen_Skill3")
		{
			each->SetLoop(false);
			animator->PushAnimation(each);
			m_baseUnitAnimations.m_skillThreeAnimation = each;
			m_unitComponent->RegisterSkillAnimation(Unit::SkillEnum::BossSkillThree, each);
		}
		//if (each->GetName() == L"Rig_Robin_arpbob|Ani_Robin_Idle")
		//{
		//	each->SetLoop(true);
		//	animator->PushAnimation(each);
		//	m_baseUnitAnimations.m_skillFourAnimation = each;
		//	m_unitComponent->RegisterSkillAnimation(Unit::SkillEnum::BossSkillFour, each);
		//}
	}
	m_unitComponent->unitAnimations = m_baseUnitAnimations;
	SetUnitAnimationFunction();
	return m_unitComponent;
}

void BossProductor::SetCursorComponent(CursorDetector* p_com)
{
	//m_unitComponent->m_cursorDetectorComponent = p_com;
}

