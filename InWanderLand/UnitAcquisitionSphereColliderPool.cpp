#include "UnitAcquisitionSphereColliderPool.h"

std::weak_ptr<UnitAcquisitionSphereCollider> UnitAcquisitionSphereColliderPool::Borrow(std::weak_ptr<Unit> owner)
{
    auto ret = GameObjectPool<UnitAcquisitionSphereCollider>::Borrow();
    ret.lock()->SetActive(true);
    ret.lock()->owner = owner;
    return ret;
}

void UnitAcquisitionSphereColliderPool::Return(std::weak_ptr<UnitAcquisitionSphereCollider> collider)
{
    borrowedList.erase(collider);
    collider.lock()->SetActive(false);
    GameObjectPool<UnitAcquisitionSphereCollider>::Return(collider);
}

void UnitAcquisitionSphereColliderPool::OnContentsStop()
{
    for (auto each : borrowedList)
    {
        each.lock()->SetActive(false);
        GameObjectPool<UnitAcquisitionSphereCollider>::Return(each);
    }
    borrowedList.clear();
    GetComponent()->SetActive(false);
}

int UnitAcquisitionSphereColliderPool::poolObjectsSize()
{
    return GameObjectPool<UnitAcquisitionSphereCollider>::poolObjectsSize();
}

int UnitAcquisitionSphereColliderPool::expendableObjectsSize()
{
    return GameObjectPool<UnitAcquisitionSphereCollider>::expendableObjectsSize();
}
