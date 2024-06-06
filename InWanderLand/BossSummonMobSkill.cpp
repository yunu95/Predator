#include "InWanderLand.h"
#include "BossSummonMobSkill.h"

POD_BossSummonMobSkill BossSummonMobSkill::pod = POD_BossSummonMobSkill();

coroutine::Coroutine BossSummonMobSkill::operator()()
{

	co_return;
}

void BossSummonMobSkill::OnInterruption()
{
}
