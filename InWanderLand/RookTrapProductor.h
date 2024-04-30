#pragma once
#include "UnitProductor.h"

class RookTrapProductor : public UnitProductor, public SingletonComponent<RookTrapProductor>
{
public:
	virtual void SetUnitData() override;
	virtual void SingletonInitializer() override;
	virtual Unit* CreateUnit(Vector3d startPos) override;
};

