#include "InWanderLand.h"
#include "RobinTauntSkill.h"

POD_RobinTauntSkill RobinTauntSkill::pod = POD_RobinTauntSkill();

coroutine::Coroutine RobinTauntSkill::operator()()
{
    owner.lock()->PlayAnimation(UnitAnimType::Taunt, true);
    coroutine::ForSeconds forSeconds{ pod.robinESkillPlayTime };
    tauntCollider = UnitAcquisitionSphereColliderPool::SingleInstance().Borrow(owner.lock());
    tauntCollider.lock()->SetRadius(pod.robinESkillRadius);
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
            each->Damaged(owner, pod.robinESkillDamage);

            /// 도발
            /// 도발 대상은 robinESkillTauntTime 동안 Robin 공격하게 되어야 함
        }
    }
    OnInterruption();
    co_return;
}

void RobinTauntSkill::OnInterruption()
{
    tauntCollider.lock()->SetRadius(0.5);
    UnitAcquisitionSphereColliderPool::SingleInstance().Return(tauntCollider);
}
