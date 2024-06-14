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

float PassiveUrsula::GetDuration()
{
    return PlayerController::Instance().IsSkillUpgraded(SkillUpgradeType::URSULA_PASSIVE_ENHANCE) ? pod.durationUpgraded : pod.duration;
}

float PassiveUrsula::GetAttackSpeedBonusPerStack()
{
    return PlayerController::Instance().IsSkillUpgraded(SkillUpgradeType::URSULA_PASSIVE_ENHANCE) ? pod.attackDamageBonusPerStackUpgraded : pod.attackDamageBonusPerStack;
}

float PassiveUrsula::GetAttackDamageBonusPerStack()
{
    return PlayerController::Instance().IsSkillUpgraded(SkillUpgradeType::URSULA_PASSIVE_ENHANCE) ? pod.attackDamageBonusPerStack : pod.attackDamageBonusPerStackUpgraded;
}
