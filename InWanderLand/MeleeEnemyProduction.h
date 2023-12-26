#pragma once
#include "UnitProductionOrder.h"

class MeleeEnemyProduction : public UnitProductionOrder
{
public:
	virtual void SetUnitData(GameObject* fbxObject) override;
	virtual GameObject* CreateUnitWithOrder() override;
};

