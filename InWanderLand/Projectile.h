#pragma once
#include "YunutyEngine.h"
#include "UnitAcquisitionSphereCollider.h"
#include "ITacticObject.h"
#include "ProjectilePool.h"

class Unit;
// 평타에 해당하는 무유도 발사체, 적군에 닿으면 피해를 입히며, 땅에 부딪히면 사라진다.
class Projectile : public UnitAcquisitionSphereCollider, public PermanentObservee, public ITacticObject
{
protected:
    //virtual void Update() override;
    //virtual void OnEnemyEnter(Unit* unit) override;
    //std::weak_ptr<Unit> owner;
    Vector3d speed;
    std::weak_ptr<Unit> opponentUnit;
    float damage;
    bool traveling = false;
    void SetSpeed(Vector3d speed);
    virtual void OnContentsPlay()override {};
    virtual void OnContentsStop() = 0;
    virtual Component* GetComponent() { return this; };

    virtual void OnPause() override {}
    virtual void OnResume() override {}

public:
    //void SetSpeed(Vector3d startPosition, Vector3d endPosition, float projectileSpeed);
    virtual void Init() = 0;
    friend ProjectilePool<Projectile>;
};
