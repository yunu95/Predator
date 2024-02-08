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
    virtual void SingletonInitializer()override;
protected:
public:
    virtual void SetUnitData(GameObject* fbxObject, NavigationField* navField, Vector3d startPosition) override;
    virtual GameObject* CreateUnitWithOrder() override;
};