#pragma once
#include "YunutyEngine.h"

class Projectile;
// ProjectilePool은 fbx 이름별로 여러가지 풀들을 갖고 있다.
// ProjectilePool은 풀 집합체와 같다고 할 수 있다.
class ProjectilePool : public SingletonClass<ProjectilePool>
{
public:
    std::weak_ptr<Projectile> Borrow(string fbxname);
    void Return(std::weak_ptr<Projectile>);
private:
    class PoolByMesh : public GameObjectPool<Projectile>
    {
    public:
        string fbxname;
        virtual void ObjectInitializer(std::weak_ptr<Projectile> unit) override;
    };
    std::unordered_map<string, std::shared_ptr<PoolByMesh>> poolsByFBX;
    std::unordered_map<std::weak_ptr<Projectile>, std::weak_ptr<PoolByMesh>> poolsByProjectile;
};
