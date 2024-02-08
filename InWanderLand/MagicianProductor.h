#pragma once
#include "UnitProductor.h"
#include "SingletonComponent.h"

class MagicianProductor : public UnitProductor, public SingletonComponent<MagicianProductor>
{
private:
	float m_QSkillProjectileRadius;
	float m_QSkillFieldRadius;

	float m_WSkillProjectileRadius;
	float m_WSkillFieldRadius;
protected:
public:
	virtual void SetUnitData() override;
	virtual void SingletonInitializer() override;
	virtual GameObject* CreateUnit(Vector3d startPos) override;
};