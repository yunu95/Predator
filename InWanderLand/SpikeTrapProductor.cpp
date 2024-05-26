#include "SpikeTrapProductor.h"
#include "SingleNavigationField.h"
#include "SpikeSkillSystem.h"
#include "DebugMeshes.h"
#include "Dotween.h"
#include "DamageOnlyComponent.h"

void SpikeTrapProductor::SetUnitData()
{
	m_objectName = "SpikeTrap";
	m_unitType = Unit::UnitType::SpikeTrap;
	m_unitSide = Unit::UnitSide::Enemy;

	m_maxHealth = 1;
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
	m_unitGameObject = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("SM_Spike01");
	m_unitGameObject->GetTransform()->SetWorldPosition(startPos);

	/// UnitComponent 추가
	m_unitComponent = m_unitGameObject->AddComponent<Unit>();
	UnitProductor::SetUnitComponentMembers();


#pragma endregion

	AddDotweenComponent();

#pragma region Spike Trap Damage System
	float attackColliderLength = 3.0f * UNIT_LENGTH;

	auto skillOneColliderObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	auto skillOneCollider = skillOneColliderObject->AddComponent<physics::BoxCollider>();
	skillOneCollider->SetHalfExtent({ attackColliderLength * 0.5f, attackColliderLength * 0.5f, attackColliderLength * 0.5f });

	auto damageComponent = skillOneColliderObject->AddComponent<DamageOnlyComponent>();
	damageComponent->SetSkillOwnerUnit(m_unitComponent);
	damageComponent->SetSkillDamage(5.0f);

	auto skillOneDebugMesh = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	AttachDebugMesh(skillOneDebugMesh, DebugMeshType::Cube, yunuGI::Color::red(), true);
	skillOneDebugMesh->GetTransform()->SetWorldScale({ attackColliderLength ,attackColliderLength ,attackColliderLength });

	auto eliteSkillSystem = m_unitGameObject->AddComponent<SpikeSkillSystem>();
	skillOneColliderObject->SetParent(m_unitGameObject);
	skillOneDebugMesh->SetParent(m_unitGameObject);
	eliteSkillSystem->SetSpikeSkillRequirment(skillOneColliderObject, skillOneDebugMesh);
#pragma endregion

	m_unitComponent->unitAnimations = m_baseUnitAnimations;
	SetUnitSkillFunctionToAnimation();
	return m_unitComponent;
}

