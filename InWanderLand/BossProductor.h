#pragma once
#include "UnitProductor.h"

class BossProductor : public UnitProductor, public SingletonComponent<BossProductor>
{
public:
	virtual void SetUnitData() override;
	virtual void SingletonInitializer() override;
	virtual Unit* CreateUnit(Vector3d startPos) override;
};

