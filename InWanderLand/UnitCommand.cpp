#include "UnitCommand.h"
#include "Unit.h"

UnitCommand::UnitCommand(Unit* unit, Vector3d expectedPos)
	: unit(unit), expectedPos(expectedPos), isDone(false)
{
	this->playerType = static_cast<PlayerCharacterType::Enum>(unit->GetUnitTemplateData().pod.playerUnitType.enumValue);
}

UnitCommand::~UnitCommand()
{

}
