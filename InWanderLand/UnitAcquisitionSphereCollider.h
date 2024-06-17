#pragma once
#include "YunutyEngine.h"
#include "UnitAcquisitionCollider.h"

class Unit;
class UnitAcquisitionSphereCollider : public physics::SphereCollider, public UnitAcquisitionCollider
{
public:
    UnitAcquisitionSphereCollider();
    void SetRadius(float radius);
    void SetColor(yunuGI::Color color);
    virtual void OnEnable() override { physics::SphereCollider::OnEnable(); };
    virtual void Update() override;
protected:
    GameObject* debugMesh = nullptr;
private:
};
