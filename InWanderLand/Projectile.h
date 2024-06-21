#pragma once
#include "YunutyEngine.h"
#include "UnitAcquisitionSphereCollider.h"
#include "ITacticObject.h"
#include "DamageType.h"
//#include "ProjectilePool.h"

class Unit;
class ProjectilePool;
// 평타에 해당하는 무유도 발사체, 적군에 닿으면 피해를 입히며, 땅에 부딪히면 사라진다.
class Projectile : public UnitAcquisitionSphereCollider, public PermanentObservee, public ITacticObject
{
protected:
    virtual void Update() override;
    ProjectileType::Enum projectileType;
    std::weak_ptr<Unit> homingTarget;
    Vector3d relativePositionFromTarget;
    Vector3d speed;
    std::weak_ptr<Unit> opponentUnit;
    float damage;
    DamageType damageType;
    float lifetime = 0;
    bool traveling = false;
    void SetSpeed(Vector3d speed);
    virtual void OnContentsPlay()override {};
    virtual void OnContentsStop();
    virtual Component* GetComponent() { return this; };

    virtual void OnPause() override {}
    virtual void OnResume() override {}

public:
    void SetDamageType(DamageType damageType);
    void SetSpeed(Vector3d startPosition, Vector3d endPosition, float projectileSpeed);
    friend ProjectilePool;
    friend Unit;
};
