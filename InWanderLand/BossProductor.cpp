#include "BossProductor.h"

void BossProductor::SetUnitData()
{
	m_objectName = "Boss";
	m_unitType = Unit::UnitType::Boss;
	m_unitSide = Unit::UnitSide::Enemy;

	m_healthPoint = 250;
	m_manaPoint = 100;

	m_autoAttackDamage = 10;
	m_criticalHitProbability = 0.2f;
	m_criticalHitMultiplier = 1.5f;

	m_defensePoint = 15;
	m_dodgeProbability = 0.2f;
	m_criticalDamageDecreaseMultiplier = 0.2f;

	m_maxAggroNumber = 2;

	m_idRadius = 4.0f * lengthUnit;
	m_atkRadius = 1.7f * lengthUnit;
	m_unitSpeed = 4.5f;

	m_attackDelay = 1.0f;

	m_navField = &SingleNavigationField::Instance();

	qSkillPreviewType = SkillPreviewSystem::SkillPreviewMesh::OnlyPath;
	wSkillPreviewType = SkillPreviewSystem::SkillPreviewMesh::None;

	m_unitFbxName = "SKM_Robin";
}

void BossProductor::SingletonInitializer()
{
	SetUnitData();
}

Unit* BossProductor::CreateUnit(Vector3d startPos)
{

}
