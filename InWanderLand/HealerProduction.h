#pragma once
#include "UnitProductionOrder.h"
class HealerProduction : public UnitProductionOrder
{
public:
	virtual void SetUnitData(GameObject* fbxObject, NavigationField* navField, Vector3d startPosition) override;
	virtual GameObject* CreateUnitWithOrder() override;
};

