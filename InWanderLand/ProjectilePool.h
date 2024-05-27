#include "YunutyEngine.h"

class Projectile;
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
        //virtual void OnBorrow(std::weak_ptr<Projectile> unit) override;
    };
    std::unordered_map<string, std::shared_ptr<PoolByMesh>> poolsByFBX;
    std::unordered_map<std::weak_ptr<Projectile>, std::weak_ptr<PoolByMesh>> poolsByProjectile;
};
