#pragma once
#include "UnitProductor.h"
#include "SingletonComponent.h"

class RangedEnemyProductor : public UnitProductor, public SingletonComponent<RangedEnemyProductor>
{
public:
	virtual void SetUnitData() override;
	virtual void SingletonInitializer() override;
	virtual Unit* CreateUnit(Vector3d startPos) override;
};

