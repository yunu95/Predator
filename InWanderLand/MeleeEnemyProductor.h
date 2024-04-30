#pragma once
#include "UnitProductor.h"

class MeleeEnemyProductor : public UnitProductor, public SingletonComponent<MeleeEnemyProductor>
{
private:
	bool isEliteMonster{ false };
public:
	virtual void SetUnitData() override;
	virtual void SingletonInitializer() override;
	virtual Unit* CreateUnit(Vector3d startPos) override;
	virtual void MappingUnitData(application::editor::POD_Unit_TemplateData p_podData) override;
};