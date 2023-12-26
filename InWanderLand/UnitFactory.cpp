#include "UnitFactory.h"
#include "Unit.h"
#include "RangeSystem.h"
#include "PlayerController.h"
#include "FSM.h"
#include "UnitProductionOrder.h"

void UnitFactory::OrderCreateUnit(yunutyEngine::GameObject* orderedUnitObject, yunutyEngine::NavigationField* navField, Vector3d startPosition)
{
	orderedUnitObject->GetTransform()->SetWorldPosition(startPosition);
	orderedUnitObject->GetTransform()->scale = Vector3d(0.4, 0.4, 0.4);

	/// 3. NavigationAgent Component Ãß°¡
	auto unitNavigationComponent = orderedUnitObject->AddComponent<NavigationAgent>();
	unitNavigationComponent->AssignToNavigationField(navField);
	unitNavigationComponent->SetRadius(0.1f);
}

