#include "UnitCommand.h"

UnitCommand::UnitCommand(Unit* unit, Vector3d targetPos, Vector3d prevPos)
	: unit(unit), targetPos(targetPos),prevPos(prevPos), isDone(false)
{

}

UnitCommand::~UnitCommand()
{

}
