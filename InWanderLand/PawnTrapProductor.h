#pragma once
#include "UnitProductor.h"

class PawnTrapProductor : public UnitProductor, public SingletonComponent<PawnTrapProductor>
{
public:
	virtual void SetUnitData() override;
	virtual void SingletonInitializer() override;
	virtual Unit* CreateUnit(Vector3d startPos) override;
};

