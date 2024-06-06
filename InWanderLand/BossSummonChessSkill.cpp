#include "BossSummonChessSkill.h"

POD_BossSummonChessSkill BossSummonChessSkill::pod = POD_BossSummonChessSkill();

coroutine::Coroutine BossSummonChessSkill::operator()()
{

	co_return;
}

void BossSummonChessSkill::OnInterruption()
{
}
