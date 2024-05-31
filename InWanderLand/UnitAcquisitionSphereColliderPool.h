#pragma once
#include "UnitAcquisitionSphereCollider.h"
#include "GameObjectPool.h"

class UnitAcquisitionSphereColliderPool :
    private GameObjectPool<UnitAcquisitionSphereCollider>
    , public SingletonClass<UnitAcquisitionSphereColliderPool>
{
public:
    std::weak_ptr<UnitAcquisitionSphereCollider> Borrow(std::weak_ptr<Unit> owner);
    void Return(std::weak_ptr<UnitAcquisitionSphereCollider> collider);
    int poolObjectsSize();
    int expendableObjectsSize();
private:
};
