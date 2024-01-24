#include "InWanderLand.h"
#include "UnitFactory.h"
#include "Unit.h"
#include "RangeSystem.h"
#include "PlayerController.h"
#include "FSM.h"
#include "UnitProductionOrder.h"

void UnitFactory::OrderCreateUnit(yunutyEngine::GameObject* orderedUnitObject, yunutyEngine::NavigationField* navField, Vector3d startPosition)
{
	orderedUnitObject->GetTransform()->SetWorldPosition(startPosition);

}

