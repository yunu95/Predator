#include "MagicianProduction.h"

void MagicianProduction::SetUnitData(GameObject* fbxObject)
{
	m_objectName = "Magician";
	m_unitType = Unit::UnitType::Magician;
	m_unitSide = Unit::UnitSide::Player;
	m_hp = 80;
	m_ap = 20;
	m_idRadius = 10.0f;
	m_atkRadius = 4.5f;
	m_unitSpeed = 1.0f;
}

yunutyEngine::GameObject* MagicianProduction::CreateUnitWithOrder()
{
	auto unitGameObject = UnitProductionOrder::CreateUnitWithOrder();
	return unitGameObject;
}
