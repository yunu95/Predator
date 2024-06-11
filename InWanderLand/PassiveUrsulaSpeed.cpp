#include "PassiveUrsulaSpeed.h"
#include "InWanderLand.h"

POD_PassiveUrsulaSpeed PassiveUrsula::pod;
void PassiveUrsula::Init(std::weak_ptr<Unit> owner)
{
    owner.lock()->onAttack.AddCallback([this, owner](std::weak_ptr<Unit>)
        {
            owner.lock()->ApplyBuff(UnitBuffUrsula{});
        });
    owner.lock()->onDamaged.AddCallback([this, owner]()
        {
            owner.lock()->EraseBuff(UnitBuffType::UrsulaBuff);
        });
}
