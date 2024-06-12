#include "InWanderLand.h"
#include "EnemyImpaleSkill.h"

POD_EnemyImpaleSkill EnemyImpaleSkill::pod = POD_EnemyImpaleSkill();

struct Spear
{
    Vector2d position;
    float timeOffset;
};
const std::vector<Spear> SpearsInfo()
{
    std::vector<Spear> spears;
    const auto& gc = GlobalConstant::GetSingletonInstance().pod;
    // 타원에 빽뺵하게 창을 생성
    // y는 캐릭터 기준 전방방향, x는 우측방향
    float ovalHeight = (gc.impaleSkillRange - gc.impaleSkillFirstSpearOffset) * 0.5f;
    float ovalWidth = gc.impaleSkillWidth * 0.5;
    float ovalHeightSqr = ovalHeight * ovalHeight;
    float ovalWidthSqr = ovalWidth * ovalWidth;

    float currX{ 0 };
    float currY{ -ovalHeight };
    float currOvalWidth = 0;
    while (currY < ovalHeight)
    {
        const float& noise = gc.impaleSkillAriseDistanceNoisePerSpear;
        float x = { currX + math::Random::GetRandomFloat(noise) };
        float y = { currY + math::Random::GetRandomFloat(noise) };
        if (x >= currOvalWidth)
        {
            currY += gc.impaleSkillAriseDistancePerSpear;
            currOvalWidth = ovalWidth * sqrtf(1 - currY * currY / ovalHeightSqr);
            currX = -currOvalWidth;
        }
        else
        {
            currX += gc.impaleSkillAriseDistancePerSpear;
            if (currX >= -currOvalWidth)
            {
                spears.push_back({ { x, y}, 0 });
                spears.rbegin()->timeOffset = math::Random::GetRandomFloat(0, gc.impaleSkillAriseTimeNoisePerSpear);
            }
        }
    }
    std::for_each(spears.begin(), spears.end(), [=](Spear& a) { a.position.y += gc.impaleSkillFirstSpearOffset + ovalHeight; });
    std::sort(spears.begin(), spears.end(), [](const Spear& a, const Spear& b) { return a.timeOffset < b.timeOffset; });
    return spears;
}
// 창이 한번 불쑥 튀어나왔다가 다시 꺼지는 사이클
coroutine::Coroutine EnemyImpaleSkill::SpearArise(std::weak_ptr<EnemyImpaleSkill> skill, std::weak_ptr<ManagedFBX> fbx, Vector2d pos)
{
    fbx = FBXPool::SingleInstance().Borrow(wanderResources::GetFBXName(wanderResources::WanderFBX::IMPALING_SPIKE));
    std::weak_ptr<UnitAcquisitionSphereCollider> collider = UnitAcquisitionSphereColliderPool::Instance().Borrow(skill.lock()->owner);
    auto forward = owner.lock()->GetTransform()->GetWorldRotation().Forward();
    auto right = owner.lock()->GetTransform()->GetWorldRotation().Right();
    auto worldPos = owner.lock()->GetTransform()->GetWorldPosition() + forward * pos.y + right * pos.x;
    fbx.lock()->GetTransform()->SetWorldPosition(worldPos);
    const auto& gc = GlobalConstant::GetSingletonInstance().pod;
    collider.lock()->SetRadius(0.01);
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
            Vector3d knockBackDest = each->GetTransform()->GetWorldPosition() + (each->GetTransform()->GetWorldPosition() - worldPos).Normalized() * gc.impaleSkillKnockbackDistance;
            each->KnockBack(knockBackDest, gc.impaleSkillKnockbackDuration);
        }
    }

    coroutine::ForSeconds forSeconds{ gc.impaleSkillDurationPerSpear };
    while (forSeconds.Tick())
    {
        float heightAlpha = std::sinf(forSeconds.ElapsedNormalized() * math::PI);
        float yDelta = math::LerpF(gc.impaleSkillMinHeightPerSpear, gc.impaleSkillMaxHeightPerSpear, heightAlpha);
        fbx.lock()->GetTransform()->SetWorldPosition(worldPos + Vector3d::up * yDelta);
        co_await std::suspend_always{};
    }
    UnitAcquisitionSphereColliderPool::Instance().Return(collider);
    FBXPool::SingleInstance().Return(fbx);
    co_return;
}
coroutine::Coroutine EnemyImpaleSkill::operator()()
{
    auto disableNavAgent = owner.lock()->referenceDisableNavAgent.Acquire();
    auto enableNavObstacle = owner.lock()->referenceEnableNavObstacle.Acquire();
    // 창이 생성되는 시간 오프셋은 유닛으로부터의 거리와 정비례한다.
    const auto& gc = GlobalConstant::GetSingletonInstance().pod;
    owner.lock()->PlayAnimation(UnitAnimType::Slam);
    co_yield coroutine::WaitForSeconds{ 1.2f };
    coroutine::ForSeconds forSeconds{ gc.impaleSkillDuration };
    for (auto& each : SpearsInfo())
    {
        while (each.timeOffset > forSeconds.Elapsed())
        {
            forSeconds.Tick();
            co_await std::suspend_always{};
        }
        std::weak_ptr<ManagedFBX> fbx;
        owner.lock()->StartCoroutine(SpearArise(std::dynamic_pointer_cast<EnemyImpaleSkill>(selfWeakPtr.lock()), fbx, each.position));
    }
    while (forSeconds.Tick()) { co_await std::suspend_always{}; }
    owner.lock()->PlayAnimation(UnitAnimType::Idle);
    co_yield coroutine::WaitForSeconds{ 0.3f };

    co_return;
}

void EnemyImpaleSkill::OnInterruption()
{
    UnitAcquisitionSphereColliderPool::Instance().Return(knockbackCollider);
}
