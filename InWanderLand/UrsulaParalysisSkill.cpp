#include "InWanderLand.h"
#include "UrsulaParalysisSkill.h"

#include <math.h>


POD_UrsulaParalysisSkill UrsulaParalysisSkill::pod = POD_UrsulaParalysisSkill();

coroutine::Coroutine UrsulaParalysisSkill::operator()()
{
    auto blockFollowingNavigation = owner.lock()->referenceBlockFollowingNavAgent.Acquire();
    auto blockAnimLoop = owner.lock()->referenceBlockAnimLoop.Acquire();
    auto disableNavAgent = owner.lock()->referenceDisableNavAgent.Acquire();

    owner.lock()->PlayAnimation(UnitAnimType::Skill2, true);
    auto animator = owner.lock()->GetAnimator();
    auto anim = wanderResources::GetAnimation(owner.lock()->GetFBXName(), UnitAnimType::Skill2);
    coroutine::ForSeconds forSeconds{ anim->GetDuration() };
    skillCollider = UnitAcquisitionSphereColliderPool::SingleInstance().Borrow(owner.lock());
    skillCollider.lock()->SetRadius(pod.skillRadius);

    skillCollider.lock()->GetTransform()->SetWorldPosition(targetPos);
    co_await std::suspend_always{};

    bool hit = false;
    while (!hit)
    {
        auto curFrame = animator.lock()->GetCurrentFrame();

        /// 애니메이션은 몇 번 나눠서 철썩 철썩 느낌인데,
        /// 이펙트가 한 번에 모으는 형태로 기억함
        /// 알아보고 수정 필요
        if (curFrame >= 48)
        {
            hit = true;
        }

        if (hit)
        {
            co_await std::suspend_always{};
            for (auto& each : skillCollider.lock()->GetEnemies())
            {
                each->KnockBack(targetPos, anim->GetDuration() - forSeconds.ElapsedNormalized());
                each->Damaged(owner, pod.skillDamage);
            }
        }
        co_await std::suspend_always{};
    }
    
    while (forSeconds.Tick())
    {
        co_await std::suspend_always{};
    }

    /// 마지막으로 경직
    for (auto& each : skillCollider.lock()->GetEnemies())
    {
        each->Paralyze(pod.skillParalysisTime);
    }

    disableNavAgent.reset();
    blockFollowingNavigation.reset();
    owner.lock()->Relocate(owner.lock()->GetTransform()->GetWorldPosition());

    OnInterruption();
    co_return;
}

void UrsulaParalysisSkill::OnInterruption()
{
    skillCollider.lock()->SetRadius(0.5);
    UnitAcquisitionSphereColliderPool::SingleInstance().Return(skillCollider);
}
