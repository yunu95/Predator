#include "InWanderLand.h"
#include "RobinChargeSkill.h"

coroutine::Coroutine RobinChargeSkill::operator()()
{
    GlobalConstant::GetSingletonInstance().pod.robinQSkillDamageRush;
    co_return;
}
