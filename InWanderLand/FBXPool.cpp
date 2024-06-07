#include "FBXPool.h"
#include "InWanderLand.h"
#include <DirectXMath.h>

// Borrow 함수는 먼저 해당 fbx 이름에 맞는 풀이 존재하는지 확인하고, 없으면 새로 만든다.
std::weak_ptr<ManagedFBX> FBXPool::Borrow(const std::string& fbxName)
{
    if (!poolsByMeshName.contains(fbxName))
    {
        auto pool = std::make_shared<PoolByMesh>();
        pool->fbxName = fbxName;
        poolsByMeshName[fbxName] = pool;
    }
    return poolsByMeshName[fbxName]->Borrow();
}

void FBXPool::Return(std::weak_ptr<ManagedFBX> fbx)
{
    poolsByFBX.at(fbx.lock().get()).lock()->Return(fbx);
}

void FBXPool::PoolByMesh::ObjectInitializer(std::weak_ptr<ManagedFBX> mesh)
{
    auto gameObj = Scene::getCurrentScene()->AddGameObjectFromFBX(fbxName);
    gameObj->SetParent(mesh.lock()->GetGameObject());
    FBXPool::SingleInstance().poolsByFBX[mesh.lock().get()] = FBXPool::SingleInstance().poolsByMeshName.at(fbxName);
    mesh.lock()->meshObject = gameObj;
}

void ManagedFBX::OnContentsStop()
{
    FBXPool::SingleInstance().Return(GetWeakPtr<ManagedFBX>());
}

std::weak_ptr<VFXAnimator> ManagedFBX::AcquireVFXAnimator()
{
    if (this->vfxAnimator.expired())
    {
        this->vfxAnimator = meshObject->AddComponentAsWeakPtr<VFXAnimator>();
    }
    return this->vfxAnimator;
}
