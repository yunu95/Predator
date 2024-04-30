#pragma once
#include "UnitProductor.h"

class TriggerTrapProductor : public UnitProductor, public SingletonComponent<TriggerTrapProductor>
{
public:
	virtual void SetUnitData() override;
	virtual void SingletonInitializer() override;
	virtual Unit* CreateUnit(Vector3d startPos) override;
};

