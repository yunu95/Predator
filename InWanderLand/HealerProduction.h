#pragma once
#include "UnitProductionOrder.h"
class HealerProduction : public UnitProductionOrder
{
private:
	float m_QSkillFieldRadius;

public:
	virtual void SetUnitData(GameObject* fbxObject, NavigationField* navField, Vector3d startPosition) override;
	virtual GameObject* CreateUnitWithOrder() override;
};

