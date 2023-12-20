#pragma once
#include "UnitProductionOrder.h"

class MagicianProduction : public UnitProductionOrder
{
public:
	virtual void SetUnitData() override;
	virtual GameObject* CreateUnitToOrder() override;
};

