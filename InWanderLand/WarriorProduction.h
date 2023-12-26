#pragma once
#include "UnitProductionOrder.h"

class WarriorProduction : public UnitProductionOrder
{

public:
	virtual void SetUnitData(GameObject* fbxObject) override;
	virtual GameObject* CreateUnitWithOrder() override;
	
};

