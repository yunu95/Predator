#pragma once
#include "YunutyEngine.h"

class Unit;
class ProjectilePool;
// 평타에 해당하는 무유도 발사체, 적군에 닿으면 피해를 입히며, 땅에 부딪히면 사라진다.
class Projectile : public Component
{
private:
    std::weak_ptr<Unit> owner;
    Vector3d speed;
    float damage;
    virtual void Update() override;
public:
    friend ProjectilePool;
};
