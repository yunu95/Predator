#pragma once
#include "UnitProductor.h"

class EnemyProductor : public UnitProductor
{
protected:
	bool isWaveUnit;

public:
	void IsCreatedByWaveSystem();
};

