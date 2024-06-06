#include "BossDebuffFieldSkill.h"

POD_BossDebuffFieldSkill BossDebuffFieldSkill::pod = POD_BossDebuffFieldSkill();

coroutine::Coroutine BossDebuffFieldSkill::operator()()
{

	co_return;
}

void BossDebuffFieldSkill::OnInterruption()
{
}
