#pragma once
#include "EnemyProductor.h"
#include "SingletonComponent.h"

class MeleeEnemyProductor : public EnemyProductor, public SingletonComponent<MeleeEnemyProductor>
{
public:
	//virtual void SetUnitData(GameObject* fbxObject, NavigationField* navField, Vector3d startPosition) override;
	//virtual GameObject* CreateUnitWithOrder() override;
	virtual void SetUnitData() override;
	virtual void SingletonInitializer() override;
	virtual GameObject* CreateUnit(Vector3d startPos) override;
};