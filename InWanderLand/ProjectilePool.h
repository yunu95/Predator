#pragma once
#include "YunutyEngine.h"
#include "Projectile.h"
#include "ProjectileType.h"
#include "InWanderLand.h"
#include <DirectXMath.h>

class Projectile;
struct ProjectileType;
struct ProjectileHoming;
// ProjectilePool은 fbx 이름별로 여러가지 풀들을 갖고 있다.
// ProjectilePool은 풀 집합체와 같다고 할 수 있다.
template <typename T>
class ProjectilePool : public SingletonClass<ProjectilePool<T>>
{
public:
    std::weak_ptr<T> Borrow(std::weak_ptr<Unit> owner, std::weak_ptr<Unit> opponent);
    void Return(std::weak_ptr<T>);
private:
    class PoolByMesh : public GameObjectPool<T>
    {
    public:
        string fbxname;
        virtual void ObjectInitializer(std::weak_ptr<T> projectile) override;
    };
    std::unordered_map<string, std::shared_ptr<PoolByMesh>> poolsByFBX;
    std::unordered_map<T*, std::weak_ptr<PoolByMesh>> poolsByProjectile;
};

template<typename T>
std::weak_ptr<T> ProjectilePool<T>::Borrow(std::weak_ptr<Unit> owner, std::weak_ptr<Unit> opponent)
{
    const std::string& fbxname = owner.lock()->GetUnitTemplateData().pod.projectile_staticFBXName;
    if (!poolsByFBX.contains(fbxname))
    {
        std::shared_ptr<PoolByMesh> pool = std::make_shared<PoolByMesh>();
        pool->fbxname = fbxname;
        poolsByFBX[fbxname] = pool;
    }
    std::weak_ptr<T> ret = poolsByFBX[fbxname]->Borrow();
    ret.lock()->owner = owner;
    const auto& v = owner.lock()->GetUnitTemplateData().pod.projectileOffset;
    DirectX::XMVECTOR offset = DirectX::XMVectorSet(v.x, v.y, v.z, 1);
    auto yunuWtm = owner.lock()->GetTransform()->GetWorldTM();
    DirectX::XMMATRIX wtm = DirectX::XMLoadFloat4x4(reinterpret_cast<const DirectX::XMFLOAT4X4*>(&yunuWtm));
    offset = DirectX::XMVector4Transform(offset, wtm);
    ret.lock()->GetTransform()->SetWorldPosition({ offset.m128_f32[0],offset.m128_f32[1],offset.m128_f32[2] });
    ret.lock()->opponentUnit = opponent;
    ret.lock()->Init();
    return ret;
}

template<typename T>
void ProjectilePool<T>::Return(std::weak_ptr<T> projectile)
{
    if (projectile.expired())
    {
        return;
    }
    projectile.lock()->traveling = false;
    poolsByProjectile.at(projectile.lock().get()).lock()->Return(projectile);
}


template<typename T>
void ProjectilePool<T>::PoolByMesh::ObjectInitializer(std::weak_ptr<T> projectile)
{
    auto gameObj = Scene::getCurrentScene()->AddGameObjectFromFBX(fbxname);
    gameObj->SetParent(projectile.lock()->GetGameObject());
    ProjectilePool::SingleInstance().poolsByProjectile[projectile.lock().get()] = ProjectilePool::SingleInstance().poolsByFBX.at(fbxname);
    projectile.lock()->SetRadius(0.0001);
}