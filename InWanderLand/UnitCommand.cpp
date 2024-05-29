#include "UnitCommand.h"

UnitCommand::UnitCommand(Unit* unit, Vector3d finalPos, Vector3d targetPos)
	: unit(unit), finalPos(finalPos), targetPos(targetPos), isDone(false)
{

}

UnitCommand::~UnitCommand()
{

}
