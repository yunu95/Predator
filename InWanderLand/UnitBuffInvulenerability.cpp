#include "UnitBuffInvulenerability.h"
#include "InWanderLand.h"

void UnitBuffInvulenerability::OnStart()
{
    invulnerabilityGuard = owner.lock()->referenceInvulnerable.Acquire();
};
