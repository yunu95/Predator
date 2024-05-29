#include "UnitBuffBleeding.h"
#include "YunutyEngine.h"
#include "Unit.h"

void UnitBuffBleeding::OnUpdate()
{
    float globalconstant = 3;
    owner.lock()->Damaged(Inflictor, globalconstant * yunutyEngine::Time::GetDeltaTime());
}
