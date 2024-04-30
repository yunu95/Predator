#pragma once
#include "UnitProductor.h"
/// <summary>
/// Boss의 세 번째 스킬인 적 유닛을 생산해내는 유닛.
/// </summary>
class EnemySummonGateProductor : public UnitProductor, public SingletonComponent<EnemySummonGateProductor>
{
private:
	bool isDamagedUnit = false;

public:
	virtual void SetUnitData() override;
	virtual void SingletonInitializer() override;
	virtual Unit* CreateUnit(Vector3d startPos) override;

	void SetUnitCanBeDamaged(bool p_boolen);
};

