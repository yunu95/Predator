#include "YunutyEngine.h"
#include "Projectile.h"

class ProjectilePool 
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
        virtual void OnBorrow(std::weak_ptr<Projectile> unit) override;
    };
    std::unordered_map<string, std::weak_ptr<PoolByMesh>> poolsByTemplate;
};
