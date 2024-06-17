#pragma once
#include "YunutyEngine.h"
#include "UnitAcquisitionCollider.h"

class Unit;
class UnitAcquisitionBoxCollider : public physics::BoxCollider, public UnitAcquisitionCollider
{
public:
    UnitAcquisitionBoxCollider();
    void SetHalfExtent(Vector3f halfExtent);
    void SetColor(yunuGI::Color color);
    virtual void OnEnable() override { physics::BoxCollider::OnEnable(); };
    virtual void Update() override;
protected:
    GameObject* debugMesh = nullptr;
private:
};
