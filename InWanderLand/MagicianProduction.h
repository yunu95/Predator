#pragma once
#include "UnitProductionOrder.h"
#include "SingletonComponent.h"

class MagicianProduction : public UnitProductionOrder, public SingletonComponent<MagicianProduction>
{
private:
	float m_QSkillProjectileRadius;
	float m_QSkillFieldRadius;

	float m_WSkillProjectileRadius;
	float m_WSkillFieldRadius;
protected:
public:
	virtual void SingletonInitializer()override;
	virtual void SetUnitData(GameObject* fbxObject, NavigationField* navField, Vector3d startPosition) override;
	virtual GameObject* CreateUnitWithOrder() override;
};