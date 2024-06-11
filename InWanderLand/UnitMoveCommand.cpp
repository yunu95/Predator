#include "UnitMoveCommand.h"
#include "SkillPreviewSystem.h"

UnitMoveCommand::UnitMoveCommand(Unit* unit, Vector3d expectedPos, std::vector<Vector3d> path, bool isForAttack)
	: UnitCommand(unit, expectedPos), path(path), isForAttack(isForAttack)
{

}

UnitMoveCommand::~UnitMoveCommand()
{
	SkillPreviewSystem::Instance().HideShowMoveEndImage(routeMesh);
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
	auto playerType = static_cast<PlayerCharacterType::Enum>(unit->GetUnitTemplateData().pod.playerUnitType.enumValue);

	switch (playerType)
	{
		case PlayerCharacterType::Robin:
		{
			this->routeMesh = SkillPreviewSystem::Instance().ShowRoute(SkillPreviewSystem::UnitType::Robin, path);
		}
		break;
		case PlayerCharacterType::Ursula:
		{
			this->routeMesh = SkillPreviewSystem::Instance().ShowRoute(SkillPreviewSystem::UnitType::Ursula, path);
		}
		break;
		case PlayerCharacterType::Hansel:
		{
			this->routeMesh = SkillPreviewSystem::Instance().ShowRoute(SkillPreviewSystem::UnitType::Hansel, path);
		}
		break;
	}

	if (isForAttack)
	{
		SkillPreviewSystem::Instance().HideShowMoveEndImage(this->routeMesh);
	}
}

void UnitMoveCommand::HidePreviewMesh()
{
	if (!isForAttack)
	{
		SkillPreviewSystem::Instance().HideShowMoveEndImage(routeMesh);
	}
	SkillPreviewSystem::Instance().DeleteRouteMesh(routeMesh);
}
