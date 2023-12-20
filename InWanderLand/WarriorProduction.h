#pragma once
#include "UnitProductionOrder.h"

class WarriorProduction : public UnitProductionOrder
{

public:
	virtual void SetUnitData() override;
	virtual GameObject* CreateUnitToOrder() override;
	
};

