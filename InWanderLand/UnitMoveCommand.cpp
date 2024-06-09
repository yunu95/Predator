#include "UnitMoveCommand.h"
#include "SkillPreviewSystem.h"

UnitMoveCommand::UnitMoveCommand(Unit* unit, Vector3d expectedPos, yunuGI::IMesh* routeMesh)
	: UnitCommand(unit, expectedPos), routeMesh(routeMesh)
{

}

UnitMoveCommand::~UnitMoveCommand()
{

}

void UnitMoveCommand::Execute()
{

}

void UnitMoveCommand::ShowPreviewMesh()
{

}

void UnitMoveCommand::HidePreviewMesh()
{
	SkillPreviewSystem::Instance().DeleteRouteMesh(routeMesh);
}
