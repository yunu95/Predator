#pragma once
#include "YunutyEngine.h"

#include "UnitCommand.h"

class Unit;
class yunuGI::IMesh;

class UnitMoveCommand : public UnitCommand
{
public:
	UnitMoveCommand(Unit* unit, Vector3d expectedPos, std::vector<Vector3d> path, bool isForAttack);
	virtual ~UnitMoveCommand();

public:
	virtual void Execute() override;
	virtual void ShowPreviewMesh() override;
	virtual void HidePreviewMesh() override;

private:
	void CalculateCommandCost();

private:
	yunuGI::IMesh* routeMesh;
	std::vector<Vector3d> path;
	bool isForAttack;
};

