#include "BossImpaleSkill.h"

POD_BossImpaleSkill BossImpaleSkill::pod = POD_BossImpaleSkill();

coroutine::Coroutine BossImpaleSkill::operator()()
{

	co_return;
}

void BossImpaleSkill::OnInterruption()
{
}
