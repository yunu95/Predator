#pragma once
#include "UnitProductionOrder.h"
class HealerProduction : public UnitProductionOrder
{
public:
	virtual void SetUnitData() override;
	virtual GameObject* CreateUnitToOrder() override;
};

