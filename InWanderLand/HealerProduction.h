#pragma once
#include "UnitProductionOrder.h"
class HealerProduction : public UnitProductionOrder
{
public:
	virtual void SetUnitData(GameObject* fbxObject) override;
	virtual GameObject* CreateUnitWithOrder() override;
};

