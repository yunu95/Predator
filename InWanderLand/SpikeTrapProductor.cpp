#include "SpikeTrapProductor.h"
#include "SingleNavigationField.h"
#include "SpikeSkillSystem.h"
#include "DebugMeshes.h"
#include "Dotween.h"

void SpikeTrapProductor::SetUnitData()
{
	m_objectName = "SpikeTrap";
	m_unitType = Unit::UnitType::SpikeTrap;
	m_unitSide = Unit::UnitSide::Enemy;

	m_healthPoint = 1;
	m_manaPoint = 100;

	m_autoAttackDamage = 15;
	m_criticalHitProbability = 0.5f;
	m_criticalHitMultiplier = 1.5f;

	m_defensePoint = 5;
	m_dodgeProbability = 0.05f;
	m_criticalDamageDecreaseMultiplier = 0.05f;

	m_maxAggroNumber = 3;

	m_idRadius = 0.0f * UNIT_LENGTH;
	m_atkRadius = 0.0f * UNIT_LENGTH;
	m_unitSpeed = 4.5f;

	m_attackDelay = 5.0f;

	m_navField = &SingleNavigationField::Instance();

	m_unitFbxName = "SKM_Monster1";
}

void SpikeTrapProductor::SingletonInitializer()
{
	SetUnitData();
}

Unit* SpikeTrapProductor::CreateUnit(Vector3d startPos)
{
#pragma region Animation Related Member Setting
	m_unitGameObject = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("SKM_Monster1");
	m_unitGameObject->GetTransform()->SetWorldPosition(startPos);

	/// UnitComponent 추가
	m_unitComponent = m_unitGameObject->AddComponent<Unit>();

	auto rsrcManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
	auto animator = m_unitGameObject->GetComponent<yunutyEngine::graphics::Animator>();
	auto& animList = rsrcManager->GetAnimationList();
	for (auto each : animList)
	{
		if (each->GetName() == L"Ani_Monster1_Idle")
		{
			m_baseUnitAnimations.m_idleAnimation = each;
			m_baseUnitAnimations.m_idleAnimation->SetLoop(true);
			animator->GetGI().PushAnimation(m_baseUnitAnimations.m_idleAnimation);
			animator->GetGI().Play(m_baseUnitAnimations.m_idleAnimation);
		}
		else if (each->GetName() == L"Ani_Monster1_Walk")
		{
			m_baseUnitAnimations.m_walkAnimation = each;
			m_baseUnitAnimations.m_walkAnimation->SetLoop(true);
			animator->GetGI().PushAnimation(m_baseUnitAnimations.m_walkAnimation);
		}
		else if (each->GetName() == L"Ani_Monster1_Attack")
		{
			m_baseUnitAnimations.m_attackAnimation = each;
			m_baseUnitAnimations.m_attackAnimation->SetLoop(false);
			animator->GetGI().PushAnimation(m_baseUnitAnimations.m_attackAnimation);
		}
		if (each->GetName() == L"Ani_Monster1_Skill")
		{
			m_baseUnitAnimations.m_paralysisAnimation = each;
			m_baseUnitAnimations.m_paralysisAnimation->SetLoop(false);
			animator->GetGI().PushAnimation(m_baseUnitAnimations.m_paralysisAnimation);
		}
		if (each->GetName() == L"Ani_Monster1_Skill")
		{
			m_baseUnitAnimations.m_deathAnimation = each;
			m_baseUnitAnimations.m_deathAnimation->SetLoop(false);
			animator->GetGI().PushAnimation(m_baseUnitAnimations.m_deathAnimation);
		}
		/// Skill Animation
		if (each->GetName() == L"Ani_Monster1_Skill")
		{
			each->SetLoop(false);
			animator->GetGI().PushAnimation(each);
			m_unitComponent->RegisterSkillAnimation(Unit::SkillEnum::BossSkillOne, each);
		}
	}
#pragma endregion

	AddDotweenComponent();

#pragma region Spike Trap Damage System
	float attackColliderLength = 3.0f * UNIT_LENGTH;

	auto skillOneColliderObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	auto skillOneCollider = skillOneColliderObject->AddComponent<physics::BoxCollider>();
	skillOneCollider->SetHalfExtent({ attackColliderLength * 0.5f, attackColliderLength * 0.5f, attackColliderLength * 0.5f });

	auto skillOneDebugMesh = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	AttachDebugMesh(skillOneDebugMesh, DebugMeshType::Cube, yunuGI::Color::red(), true);
	skillOneDebugMesh->GetTransform()->SetWorldScale({ attackColliderLength ,attackColliderLength ,attackColliderLength });

	auto eliteSkillSystem = m_unitGameObject->AddComponent<SpikeSkillSystem>();
	skillOneColliderObject->SetParent(m_unitGameObject);
	skillOneDebugMesh->SetParent(m_unitGameObject);
	eliteSkillSystem->SetSpikeSkillRequirment(skillOneColliderObject, skillOneDebugMesh, m_unitComponent->dotween);
#pragma endregion

	auto skinnedMeshRenderer = m_unitGameObject->GetChildren()[0]->GetComponent<yunutyEngine::graphics::SkinnedMesh>();
	auto material = skinnedMeshRenderer->GetGI().GetMaterial();
	auto clonedMaterial = graphics::Renderer::SingleInstance().GetResourceManager()->CloneMaterial(L"Green", material);
	clonedMaterial->SetColor(yunuGI::Color::green());
	skinnedMeshRenderer->GetGI().SetMaterial(0, clonedMaterial);

	return m_unitComponent;
}

