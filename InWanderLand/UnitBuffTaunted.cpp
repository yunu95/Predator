#include "UnitBuffTaunted.h"
#include "InWanderLand.h"

UnitBuffType UnitBuffTaunted::GetBuffType()
{
    return UnitBuffType::Taunted;
}

UIEnumID UnitBuffTaunted::GetUIEnumID()
{
    return UIEnumID::CharInfo_Buff_Taunted;
}

void UnitBuffTaunted::OnUpdate()
{
    if (!blockPendingCommand.get() && owner.lock()->currentTargetUnit.lock().get() == inflictor.lock().get())
    {
        blockPendingCommand = owner.lock()->referenceBlockPendingOrder.Acquire();
    }
}

UnitBuffTaunted::UnitBuffTaunted(std::weak_ptr<Unit> inflictor)
{
    durationLeft = RobinTauntSkill::pod.skillTauntTime;
    this->inflictor = inflictor;
}
