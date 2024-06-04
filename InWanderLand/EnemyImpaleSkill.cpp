#include "InWanderLand.h"
#include "EnemyImpaleSkill.h"

// 창이 한번 불쑥 튀어나왔다가 다시 꺼지는 사이클
coroutine::Coroutine EnemyImpaleSkill::SpearArise(std::weak_ptr<EnemyImpaleSkill> skill, Vector2d pos, float timeOffset)
{
    std::weak_ptr<ManagedFBX> fbx = FBXPool::SingleInstance().Borrow(wanderResources::GetFBXName(wanderResources::WanderFBX::IMPALING_SPIKE));
    std::weak_ptr<UnitAcquisitionSphereCollider> collider = UnitAcquisitionSphereColliderPool::SingleInstance().Borrow(skill.lock()->owner);
    auto forward = owner.lock()->GetTransform()->GetWorldRotation().Forward();
    auto right = owner.lock()->GetTransform()->GetWorldRotation().Right();
    auto worldPos = forward * pos.y + right * pos.x;
    fbx.lock()->GetTransform()->SetWorldPosition(worldPos);
    const auto& gc = GlobalConstant::GetSingletonInstance().pod;
    collider.lock()->SetRadius(0.001);
    collider.lock()->GetTransform()->SetWorldPosition(worldPos);
    co_await std::suspend_always{};
    if (!skill.expired())
    {
        for (auto& each : collider.lock()->GetEnemies())
        {
            if (skill.lock()->damagedUnits.contains(each))
            {
                continue;
            }
            skill.lock()->damagedUnits.insert(each);
            each->Damaged(skill.lock()->owner, gc.impaleSkillDamage);
        }
    }

    coroutine::ForSeconds forSeconds{ gc.impaleSkillDurationPerSpear };
    while (forSeconds.Tick())
    {
        float heightAlpha = std::sinf(forSeconds.ElapsedNormalized() * math::PI);
        float yDelta = math::LerpF(gc.impaleSkillMinHeightPerSpear, gc.impaleSkillMinHeightPerSpear, heightAlpha);
        fbx.lock()->GetTransform()->SetWorldPosition(worldPos + Vector3d::up * yDelta);
        co_await std::suspend_always{};
    }

    co_return;
}
coroutine::Coroutine EnemyImpaleSkill::operator()()
{
    // 창이 생성되는 시간 오프셋은 유닛으로부터의 거리와 정비례한다.
    const auto& gc = GlobalConstant::GetSingletonInstance().pod;
    owner.lock()->PlayAnimation(UnitAnimType::Slam);
    co_yield coroutine::WaitForSeconds{ 1.0f };
    coroutine::ForSeconds forSeconds{ 0.5f };

    float ovalHeight = gc.impaleSkillRange - gc.impaleSkillFirstSpearOffset;

    co_return;
}

void EnemyImpaleSkill::OnInterruption()
{
    UnitAcquisitionSphereColliderPool::SingleInstance().Return(knockbackCollider);
}
