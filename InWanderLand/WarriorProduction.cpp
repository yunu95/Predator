#include "WarriorProduction.h"

void WarriorProduction::SetUnitData()
{
	m_objectName = "Warrior";
	m_unitType = Unit::UnitType::Warrior;
	m_unitSide = Unit::UnitSide::Player;
	m_hp = 150;
	m_ap = 10;
	m_idRadius = 5.0f;
	m_atkRadius = 1.5f;
	m_unitSpeed = 1.5f;
}

GameObject* WarriorProduction::CreateUnitToOrder()
{
	auto unitGameObject = UnitProductionOrder::CreateUnitToOrder();
	return unitGameObject;
}

