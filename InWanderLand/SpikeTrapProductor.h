#pragma once
#include "UnitProductor.h"
#include "SingletonComponent.h"

class SpikeTrapProductor : public UnitProductor, public SingletonComponent<SpikeTrapProductor>
{
public:
	virtual void SetUnitData() override;
	virtual void SingletonInitializer() override;
	virtual Unit* CreateUnit(Vector3d startPos) override;
};

