#include "InWanderLand.h"
#include "UrsulaQSkill.h"

UrsulaQSkill::UrsulaQSkill()
{
    //const application::POD_GlobalConstant& gc = GlobalConstant::GetSingletonInstance().pod;

    //for (int i = 0; i < 3; i++)
    //{
    //    auto rushCollider = UnitAcquisitionSphereColliderPool::SingleInstance().Borrow(owner.lock());
    //    rushCollider.lock()->SetRadius(gc.ursulaQSkillRadius);
    //    rushCollider.lock()->GetGameObject()->SetSelfActive(false);
    //    projectiles.push_back(rushCollider.lock().get());
    //}
}

coroutine::Coroutine UrsulaQSkill::operator()()
{
    const application::POD_GlobalConstant& gc = GlobalConstant::GetSingletonInstance().pod;

    for (int i = 0; i < 3; i++)
    {
        auto rushCollider = UnitAcquisitionSphereColliderPool::SingleInstance().Borrow(owner.lock());
        rushCollider.lock()->SetRadius(gc.ursulaQSkillRadius);
        rushCollider.lock()->GetGameObject()->SetSelfActive(false);
        projectiles.push_back(rushCollider.lock().get());
        AttachDebugMesh(rushCollider.lock()->GetGameObject(), DebugMeshType::Sphere);
    }
 /*   Vector3d startPos = owner.lock()->GetTransform()->GetWorldPosition();
    Vector3d deltaPos = targetPos - owner.lock()->GetTransform()->GetWorldPosition();
    Vector3d direction = deltaPos.Normalized();
    if (deltaPos.Magnitude() > gc.ursulaQSkillMaxDistance)
    {
        deltaPos = direction * gc.ursulaQSkillMaxDistance;
    }
    Vector3d endPos = startPos + deltaPos;
    Vector3d currentPos = startPos;*/

    owner.lock()->PlayAnimation(UnitAnimType::Skill1, false);
    coroutine::ForSeconds forSeconds{ gc.ursulaQSkillShootDelay };

    projectiles[0]->SetActive(true);
    owner.lock()->StartCoroutine(ShootProjectile(projectiles[0]));
    
    float localTime{ 0.0f };
    int shootedNum{ 0 };
    int index{ 1 };

    while (shootedNum != 2)
    {
        localTime += Time::GetDeltaTime();
        if (localTime >= gc.ursulaQSkillShootDelay)
        {
            owner.lock()->StartCoroutine(ShootProjectile(projectiles[index]));
            localTime = 0.0f;
            shootedNum++;
            index++;
        }
        co_await std::suspend_always{};
    }

    OnFinishSuccessful();
    co_return;
}

coroutine::Coroutine UrsulaQSkill::ShootProjectile(UnitAcquisitionSphereCollider* projectileCollider)
{
    const application::POD_GlobalConstant& gc = GlobalConstant::GetSingletonInstance().pod;
    Vector3d startPos = owner.lock()->GetTransform()->GetWorldPosition();
    Vector3d deltaPos = targetPos - owner.lock()->GetTransform()->GetWorldPosition();
    Vector3d direction = deltaPos.Normalized();
    if (deltaPos.Magnitude() > gc.ursulaQSkillMaxDistance)
    {
        deltaPos = direction * gc.ursulaQSkillMaxDistance;
    }
    Vector3d endPos = startPos + deltaPos;
    Vector3d currentPos = startPos;

    coroutine::ForSeconds forSeconds{ static_cast<float>(deltaPos.Magnitude()) / gc.ursulaQSkillProjectileSpeed };

    while (forSeconds.Tick())
    {
        currentPos += direction * gc.ursulaQSkillProjectileSpeed * Time::GetDeltaTime();
        projectileCollider->GetTransform()->SetWorldPosition(currentPos);
        co_await std::suspend_always{};
        for (auto& each : projectileCollider->GetEnemies())
        {
            each->Damaged(owner, gc.ursulaQSkillFieldDamagePerTick);
        }
    }

    yunutyEngine::Scene::getCurrentScene()->DestroyGameObject(projectileCollider->GetGameObject());

    co_return;
}

void UrsulaQSkill::OnFinishSuccessful()
{
    projectiles.clear();
}

void UrsulaQSkill::OnFinishInterrupted()
{
    projectiles.clear();
}
