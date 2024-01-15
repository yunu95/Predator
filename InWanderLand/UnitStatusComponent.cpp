#include "InWanderLand.h"
#include "UnitStatusComponent.h"
#include "UnitTransformComponent.h"
#include "Unit.h"

void UnitStatusComponent::OnTriggerEnter(physics::Collider* collider)
{
	// Request StatusTimer To TimerPool here
	if (collider->GetGameObject()->GetComponent<UnitTransformComponent>() != nullptr &&
		collider->GetGameObject()->GetComponent<UnitTransformComponent>()->ownerObject->GetComponent<Unit>() != nullptr &&
		collider->GetGameObject()->GetComponent<UnitTransformComponent>()->ownerObject->GetComponent<Unit>()->GetUnitSide() == Unit::UnitSide::Enemy)
	{
		ApplyStatus(GetGameObject()->GetParentGameObject()->GetComponent<UnitTransformComponent>()->ownerObject->GetComponent<Unit>(),
			collider->GetGameObject()->GetComponent<UnitTransformComponent>()->ownerObject->GetComponent<Unit>());
	}
}
