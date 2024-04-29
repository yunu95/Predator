#pragma once
#include "UnitProductor.h"

class CursorDetector;

class BossProductor : public UnitProductor, public SingletonComponent<BossProductor>
{
public:
	virtual void SetUnitData() override;
	virtual void SingletonInitializer() override;
	virtual Unit* CreateUnit(Vector3d startPos) override;

	void SetCursorComponent(CursorDetector* p_com);
};

