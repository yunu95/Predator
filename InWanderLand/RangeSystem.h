#pragma once
#include "YunutyEngine.h"
/// <summary>
/// GetGameObject->GetComponent<IRangeAction>을 통해 함수호출 할 예정. 
/// </summary>
class RangeSystem : public Component
{
private:
    GameObject* m_unit;
    string m_type;
public:
    void SetUnitGameObject(GameObject* obj);
    void SetRangeType(string p_type);
public:
    virtual void OnCollisionEnter2D(const Collision2D& collision) override;
};

