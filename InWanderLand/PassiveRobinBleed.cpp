#include "PassiveRobinBleed.h"
#include "InWanderLand.h"

POD_PassiveRobinBleed PassiveRobinBleed::pod;
void PassiveRobinBleed::Init(std::weak_ptr<Unit> owner)
{
    owner.lock()->onAttackHit.AddCallback([this, owner]()
        {
            auto target = owner.lock()->GetAttackTarget();
            if (target.lock()->IsAlive())
            {
                target.lock()->ApplyBuff(UnitBuffBleeding{ owner, pod.dpsPerStack, pod.duration });
            }
        });
}
