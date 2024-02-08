#pragma once
#include "UnitProductor.h"
#include "SingletonComponent.h"

class BleedingComponent;
class KnockBackComponent;

class WarriorProductor : public UnitProductor, public SingletonComponent<WarriorProductor>
{
private:
	float m_QSkillRadius;
	float m_WSkillRadius;

public:
	virtual void SetUnitData() override;
	virtual void SingletonInitializer() override;
	virtual GameObject* CreateUnit(Vector3d startPos) override;
};

