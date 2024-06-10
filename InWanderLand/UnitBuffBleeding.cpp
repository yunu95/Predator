#include "UnitBuffBleeding.h"
#include "YunutyEngine.h"
#include "Unit.h"

void UnitBuffBleeding::OnUpdate()
{
    owner.lock()->Damaged(Inflictor, damagePerSecond * yunutyEngine::Time::GetDeltaTime());
}
