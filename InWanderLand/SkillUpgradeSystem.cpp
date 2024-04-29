#include "SkillUpgradeSystem.h"
#include "InWanderLand.h"

void SkillUpgradeSystem::SetUpgradeTarget(UIEnumID id)
{
    upgradeTarget = id;
}
bool SkillUpgradeSystem::IsUpgraded(UIEnumID id)
{
    return upgradedList.find(id) != upgradedList.end();
}
void SkillUpgradeSystem::UpgradeSkill()
{
    SetSkillPoints(skillPointsLeft - 1);
    upgradedList.insert(upgradeTarget);
}
void SkillUpgradeSystem::SetSkillPoints(int points)
{
    skillPointsLeft = points;
    UIManager::Instance().GetUIElementByEnum(UIEnumID::SkillPoint_Number)->SetNumber(skillPointsLeft);
}
int SkillUpgradeSystem::GetSkillPoints()
{
    return skillPointsLeft;
}
void SkillUpgradeSystem::IncrementSkillPoint()
{
    SetSkillPoints(skillPointsLeft + 1);
}
void SkillUpgradeSystem::Reset()
{
    upgradedList.clear();
    SetSkillPoints(5);
}
