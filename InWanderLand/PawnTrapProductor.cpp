#include "PawnTrapProductor.h"
#include "SingleNavigationField.h"
#include "ChessTrapSkillSystem.h"
#include "DebugMeshes.h"
#include "DamageOnlyComponent.h"
#include "TrapTriggerSensor.h"


void PawnTrapProductor::SetUnitData()
{
	m_objectName = "PawnTrap";
	m_unitType = Unit::UnitType::ChessTrap;
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

void PawnTrapProductor::SingletonInitializer()
{
	SetUnitData();
}

Unit* PawnTrapProductor::CreateUnit(Vector3d startPos)
{
#pragma region Animation Related Member Setting
	m_unitGameObject = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("SM_Chess_Pawn");
	m_unitGameObject->GetTransform()->SetWorldPosition(startPos);

	/// UnitComponent 추가
	m_unitComponent = m_unitGameObject->AddComponent<Unit>();
	UnitProductor::SetUnitComponentMembers();

#pragma endregion

	AddDotweenComponent();

	auto chessSkillSystem = m_unitGameObject->AddComponent<ChessTrapSkillSystem>();

	chessSkillSystem->SetChessType(ChessTrapSkillSystem::ChessPieceType::Pawn);
	chessSkillSystem->SetUnitLength(UNIT_LENGTH);

	auto colliderObjectOne = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	colliderObjectOne->AddComponent<physics::BoxCollider>();
	auto colliderObjectTwo = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	colliderObjectTwo->AddComponent<physics::BoxCollider>();

	auto damageComponentOne = colliderObjectOne->AddComponent<DamageOnlyComponent>();
	damageComponentOne->SetSkillOwnerUnit(m_unitComponent);
	damageComponentOne->SetSkillDamage(2.0f);

	auto damageComponentTwo = colliderObjectTwo->AddComponent<DamageOnlyComponent>();
	damageComponentTwo->SetSkillOwnerUnit(m_unitComponent);
	damageComponentTwo->SetSkillDamage(2.0f);

	auto debugObjectOne = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	AttachDebugMesh(debugObjectOne, DebugMeshType::Cube, yunuGI::Color::red(), true);
	auto debugObjectTwo = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	AttachDebugMesh(debugObjectTwo, DebugMeshType::Cube, yunuGI::Color::red(), true);

	chessSkillSystem->SetChessSkillRequirmentOne(colliderObjectOne, debugObjectOne);
	chessSkillSystem->SetChessSkillRequirmentTwo(colliderObjectTwo, debugObjectTwo);

	float trapTriggerRadius = 3.0f * UNIT_LENGTH;
	auto triggerSensorObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	triggerSensorObject->SetParent(m_unitGameObject);
	auto triggerCollider = triggerSensorObject->AddComponent<physics::SphereCollider>();
	triggerCollider->SetRadius(trapTriggerRadius);
	triggerSensorObject->AddComponent<TrapTriggerSensor>()->trapChessSkillSystem = chessSkillSystem;

	return m_unitComponent;
}
