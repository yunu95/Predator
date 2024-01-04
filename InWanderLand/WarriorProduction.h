#pragma once
#include "UnitProductionOrder.h"

class WarriorProduction : public UnitProductionOrder
{

public:
	virtual void SetUnitData(GameObject* fbxObject, NavigationField* navField, Vector3d startPosition) override;
	virtual GameObject* CreateUnitWithOrder() override;
	
};

