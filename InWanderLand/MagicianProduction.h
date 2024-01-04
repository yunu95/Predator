#pragma once
#include "UnitProductionOrder.h"

class MagicianProduction : public UnitProductionOrder
{
public:
	virtual void SetUnitData(GameObject* fbxObject, NavigationField* navField, Vector3d startPosition) override;
	virtual GameObject* CreateUnitWithOrder() override;
};

