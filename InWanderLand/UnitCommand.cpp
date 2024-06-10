#include "UnitCommand.h"

UnitCommand::UnitCommand(Unit* unit, Vector3d expectedPos)
	: unit(unit), expectedPos(expectedPos), isDone(false)
{

}

UnitCommand::~UnitCommand()
{

}
