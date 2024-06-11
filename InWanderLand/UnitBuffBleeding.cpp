#include "UnitBuffBleeding.h"
#include "YunutyEngine.h"
#include "Unit.h"
#include "InWanderLand.h"

void UnitBuffBleeding::OnUpdate()
{
    owner.lock()->Damaged(Inflictor, damagePerSecond * yunutyEngine::Time::GetDeltaTime());
}

void UnitBuffBleeding::OnOverlap(UnitBuff&& overlapping)
{
    UnitBuff::OnOverlap(std::move(overlapping));
    if (auto sameBleeding = dynamic_cast<UnitBuffBleeding*>(&overlapping); sameBleeding)
    {
        damagePerSecond = std::fmin(PassiveRobinBleed::pod.dpsPerStack * PassiveRobinBleed::pod.maxStack, damagePerSecond + sameBleeding->damagePerSecond);
    }
};
