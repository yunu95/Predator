#include "HealerProduction.h"

void HealerProduction::SetUnitData(GameObject* fbxObject)
{
	m_objectName = "Healer";
	m_unitType = Unit::UnitType::Healer;
	m_unitSide = Unit::UnitSide::Player;
	m_hp = 100;
	m_ap = 10;
	m_idRadius = 15.0f;
	m_atkRadius = 10.0f;
	m_unitSpeed = 2.0f;
}

yunutyEngine::GameObject* HealerProduction::CreateUnitWithOrder()
{
	auto unitGameObject = UnitProductionOrder::CreateUnitWithOrder();
	return unitGameObject;
}
