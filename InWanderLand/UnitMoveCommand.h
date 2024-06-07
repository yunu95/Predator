#pragma once
#include "YunutyEngine.h"

#include "UnitCommand.h"

class Unit;
class yunuGI::IMesh;

class UnitMoveCommand : public UnitCommand
{
public:
	UnitMoveCommand(Unit* unit, Vector3d targetPos, Vector3d prevPos,yunuGI::IMesh* routeMesh);
	virtual ~UnitMoveCommand();

public:
	virtual void Execute() override;
	virtual void ShowPreviewMesh() override;
	virtual void HidePreviewMesh() override;

private:
	class yunuGI::IMesh* routeMesh;

};

