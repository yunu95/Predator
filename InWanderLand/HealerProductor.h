#pragma once
#include "UnitProductor.h"
#include "SingletonComponent.h"

class HealerProductor : public UnitProductor, public SingletonComponent<HealerProductor>
{
private:
	float m_QSkillFieldRadius;

	float m_WSkillFieldWidth;
	float m_WSkillFieldHeight;

public:
	virtual void SetUnitData() override;
	virtual void SingletonInitializer() override;
	virtual Unit* CreateUnit(Vector3d startPos) override;
};

