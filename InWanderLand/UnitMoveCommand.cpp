#include "UnitMoveCommand.h"
#include "SkillPreviewSystem.h"

UnitMoveCommand::UnitMoveCommand(Unit* unit, Vector3d expectedPos, yunuGI::IMesh* routeMesh)
	: UnitCommand(unit, expectedPos), routeMesh(routeMesh)
{

}

UnitMoveCommand::~UnitMoveCommand()
{
	SkillPreviewSystem::Instance().DeleteRouteMesh(routeMesh);
}

void UnitMoveCommand::Execute()
{
	unit->OrderMove(expectedPos);
	unit->OnStateExitCallback()[UnitBehaviourTree::Move].AddVolatileCallback([&]() {
		this->isDone = true; 
		});
}

void UnitMoveCommand::ShowPreviewMesh()
{

}

void UnitMoveCommand::HidePreviewMesh()
{
	SkillPreviewSystem::Instance().DeleteRouteMesh(routeMesh);
}
