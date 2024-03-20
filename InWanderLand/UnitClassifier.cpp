#include "UnitClassifier.h"
#include "UnitData.h"
#include "Unit.h"
#include "UnitProductor.h"
#include "WarriorProductor.h"
#include "MagicianProductor.h"
#include "HealerProductor.h"
#include "MeleeEnemyProductor.h"

void UnitClassifier::SendPODToClassifier(application::editor::POD_Unit p_pod)
{
	UnitProductor* currentSelectedProductor{ nullptr };

	switch (static_cast<Unit::UnitType>(p_pod.templateData->pod.unitType))
	{
		case Unit::UnitType::Healer:
			currentSelectedProductor = &HealerProductor::Instance();
			break;
		case Unit::UnitType::Warrior:
			currentSelectedProductor = &WarriorProductor::Instance();
			break;
		case Unit::UnitType::Magician:
			currentSelectedProductor = &MagicianProductor::Instance();
			break;
		default:
			currentSelectedProductor = &MeleeEnemyProductor::Instance();
			break;
	}

	currentSelectedProductor->MappingUnitData(p_pod.templateData->pod);

	if (p_pod.waveData != nullptr)
	{
		currentSelectedProductor->SetUnitData();			/// mapping 잘 되면 삭제해주기
		currentSelectedProductor->CreateUnit(Vector3d(p_pod.position.x, p_pod.position.y, p_pod.position.z));
	}

	else
	{
		/// waveData의 name 이 같은 유닛끼리 모아놓아야 한다...

	}

}
