#include "BossSpinAttackSkill.h"

POD_BossSpinAttackSkill BossSpinAttackSkill::pod = POD_BossSpinAttackSkill();

coroutine::Coroutine BossSpinAttackSkill::operator()()
{

	co_return;
}

void BossSpinAttackSkill::OnInterruption()
{

}
