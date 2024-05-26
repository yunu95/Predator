#include "UnitAcquisitionSphereColliderPool.h"

std::weak_ptr<UnitAcquisitionSphereCollider> UnitAcquisitionSphereColliderPool::Borrow(std::weak_ptr<Unit> owner)
{
    auto ret = GameObjectPool<UnitAcquisitionSphereCollider>::Borrow();
    ret.lock()->owner = owner;
    return ret;
}

void UnitAcquisitionSphereColliderPool::Return(std::weak_ptr<UnitAcquisitionSphereCollider> collider)
{
    GameObjectPool<UnitAcquisitionSphereCollider>::Return(collider);
}

int UnitAcquisitionSphereColliderPool::poolObjectsSize()
{
    return GameObjectPool<UnitAcquisitionSphereCollider>::poolObjectsSize();
}

int UnitAcquisitionSphereColliderPool::expendableObjectsSize()
{
    return GameObjectPool<UnitAcquisitionSphereCollider>::expendableObjectsSize();
}
