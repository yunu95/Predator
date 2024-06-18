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

    auto wptr = poolsByMeshName[fbxName]->Borrow();
    borrowedList.insert(wptr);
    return wptr;
}

void FBXPool::Return(std::weak_ptr<ManagedFBX> fbx)
{
    if (fbx.expired())
    {
        return;
    }
    borrowedList.erase(fbx);
    poolsByFBX.at(fbx.lock().get()).lock()->Return(fbx);
}

void FBXPool::OnContentsStop()
{
    static auto eraseList = borrowedList;
    eraseList = borrowedList;
    for (auto each : eraseList)
    {
        poolsByFBX.at(each.lock().get()).lock()->Return(each);
    }
    borrowedList.clear();
    GetComponent()->SetActive(false);
}

void FBXPool::PoolByMesh::ObjectInitializer(std::weak_ptr<ManagedFBX> mesh)
{
    auto gameObj = Scene::getCurrentScene()->AddGameObjectFromFBX(fbxName);
    gameObj->SetParent(mesh.lock()->GetGameObject());
    FBXPool::Instance().poolsByFBX[mesh.lock().get()] = FBXPool::Instance().poolsByMeshName.at(fbxName);
    mesh.lock()->meshObject = gameObj;
}

void ManagedFBX::OnContentsStop()
{
    FBXPool::Instance().Return(GetWeakPtr<ManagedFBX>());
}

std::weak_ptr<VFXAnimator> ManagedFBX::AcquireVFXAnimator()
{
    if (this->vfxAnimator.expired())
    {
        this->vfxAnimator = meshObject->AddComponentAsWeakPtr<VFXAnimator>();
    }
    return this->vfxAnimator;
}
