#pragma once
#include "UnitAcquisitionSphereCollider.h"
#include "GameObjectPool.h"
#include "PermanentObservee.h"
class UnitAcquisitionSphereColliderPool :
    private GameObjectPool<UnitAcquisitionSphereCollider>
    , public SingletonComponent<UnitAcquisitionSphereColliderPool>, public Component, public PermanentObservee
{
public:
    std::weak_ptr<UnitAcquisitionSphereCollider> Borrow(std::weak_ptr<Unit> owner);
    void Return(std::weak_ptr<UnitAcquisitionSphereCollider> collider);
    virtual void OnBorrow(std::weak_ptr<UnitAcquisitionSphereCollider> comp) { borrowedList.insert(comp); }
    virtual void OnContentsStop() override;
    int poolObjectsSize();
    int expendableObjectsSize();
    virtual void ObjectInitializer(std::weak_ptr<UnitAcquisitionSphereCollider> comp) override { comp.lock()->OnEnable(); };
    virtual Component* GetComponent() { return this; }
private:
    struct CustomCompare
    {
        bool operator()(const std::weak_ptr<UnitAcquisitionSphereCollider>& lp, const std::weak_ptr<UnitAcquisitionSphereCollider>& rp) const
        {
            return lp.lock().get() > rp.lock().get();
        }
    };

    std::set<std::weak_ptr<UnitAcquisitionSphereCollider>, CustomCompare> borrowedList = std::set<std::weak_ptr<UnitAcquisitionSphereCollider>, CustomCompare>();
};
