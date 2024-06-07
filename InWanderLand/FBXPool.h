#pragma once
#include "YunutyEngine.h"

class VFXAnimator;
class FBXPool;
class ManagedFBX;

class ManagedFBX : public Component, public PermanentObservee
{
private:
    std::weak_ptr<VFXAnimator> vfxAnimator;
    GameObject* meshObject;
    virtual void OnContentsStop()override; 
    virtual Component* GetComponent() { return this; };
public:
    std::weak_ptr<VFXAnimator> AcquireVFXAnimator();
    friend ProjectilePool;
    friend FBXPool;
};

// FBXPool은 fbx 이름별로 여러가지 풀들을 갖고 있다.
// FBXPool은 풀 집합체와 같다고 할 수 있다.
class FBXPool : public SingletonClass<FBXPool>
{
public:
    std::weak_ptr<ManagedFBX> Borrow(const string& fbxName);
    void Return(std::weak_ptr<ManagedFBX>);
private:
    class PoolByMesh : public GameObjectPool<ManagedFBX>
    {
    public:
        std::string fbxName;
        virtual void ObjectInitializer(std::weak_ptr<ManagedFBX> unit) override;
    };
    std::unordered_map<std::string, std::shared_ptr<PoolByMesh>> poolsByMeshName;
    std::unordered_map<ManagedFBX*, std::weak_ptr<PoolByMesh>> poolsByFBX;
};
