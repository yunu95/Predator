#include "UnitMoveCommand.h"
#include "SkillPreviewSystem.h"

UnitMoveCommand::UnitMoveCommand(Unit* unit, Vector3d targetPos, Vector3d prevPos, yunuGI::IMesh* routeMesh)
	: UnitCommand(unit,targetPos, prevPos), routeMesh(routeMesh)
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
