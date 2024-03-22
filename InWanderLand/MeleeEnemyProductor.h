#pragma once
#include "UnitProductor.h"
#include "SingletonComponent.h"

class MeleeEnemyProductor : public UnitProductor, public SingletonComponent<MeleeEnemyProductor>
{
public:
	//virtual void SetUnitData(GameObject* fbxObject, NavigationField* navField, Vector3d startPosition) override;
	//virtual GameObject* CreateUnitWithOrder() override;
	virtual void SetUnitData() override;
	virtual void SingletonInitializer() override;
	virtual Unit* CreateUnit(Vector3d startPos) override;
	virtual void SetUnitFbxName() override;
};