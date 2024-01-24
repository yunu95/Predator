#pragma once
#include "UnitProductionOrder.h"

class BleedingComponent;
class KnockBackComponent;

class WarriorProduction : public UnitProductionOrder
{
private:
	float m_QSkillRadius;
	float m_WSkillRadius;

public:
	virtual void SetUnitData(GameObject* fbxObject, NavigationField* navField, Vector3d startPosition) override;
	virtual GameObject* CreateUnitWithOrder() override;
	
};

