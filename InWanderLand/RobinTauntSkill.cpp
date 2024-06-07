#include "InWanderLand.h"
#include "RobinTauntSkill.h"

POD_RobinTauntSkill RobinTauntSkill::pod = POD_RobinTauntSkill();

coroutine::Coroutine RobinTauntSkill::operator()()
{
    auto blockFollowingNavigation = owner.lock()->referenceBlockFollowingNavAgent.Acquire();
    auto blockAnimLoop = owner.lock()->referenceBlockAnimLoop.Acquire();
    auto disableNavAgent = owner.lock()->referenceDisableNavAgent.Acquire();

    owner.lock()->PlayAnimation(UnitAnimType::Taunt, true);
    coroutine::ForSeconds forSeconds{ pod.skillPlayTime };
    tauntCollider = UnitAcquisitionSphereColliderPool::SingleInstance().Borrow(owner.lock());
    tauntCollider.lock()->SetRadius(pod.skillRadius);
    while (forSeconds.Tick())
    {
        tauntCollider.lock()->GetTransform()->SetWorldPosition(owner.lock()->GetTransform()->GetWorldPosition());
        co_await std::suspend_always{};
        std::unordered_set<Unit*> tauntList;
        for (auto& each : tauntCollider.lock()->GetEnemies())
        {
            if (tauntList.contains(each))
            {
                continue;
            }
            tauntList.insert(each);
            each->Damaged(owner, pod.skillDamage);

            /// 도발
            /// 도발 대상은 skillTauntTime 동안 Robin 공격하게 되어야 함
        }
    }
    disableNavAgent.reset();
    blockFollowingNavigation.reset();
    owner.lock()->Relocate(owner.lock()->GetTransform()->GetWorldPosition());
    OnInterruption();
    co_return;
}

void RobinTauntSkill::OnInterruption()
{
    tauntCollider.lock()->SetRadius(0.5);
    UnitAcquisitionSphereColliderPool::SingleInstance().Return(tauntCollider);
}
