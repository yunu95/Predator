#pragma once
#include "YunutyEngine.h"
#include "UnitAcquisitionSphereCollider.h"

class Unit;
class ProjectilePool;
// 평타에 해당하는 무유도 발사체, 적군에 닿으면 피해를 입히며, 땅에 부딪히면 사라진다.
class Projectile : public UnitAcquisitionSphereCollider
{
private:
    virtual void Update() override;
    virtual void OnHostileEnter(Unit* unit) override;
    std::weak_ptr<Unit> owner;
    Vector3d speed;
    float damage;
    bool traveling = false;
public:
    void SetSpeed(Vector3d speed);
    void SetSpeed(Vector3d startPosition, Vector3d endPosition, float projectileSpeed);
    friend ProjectilePool;
};
