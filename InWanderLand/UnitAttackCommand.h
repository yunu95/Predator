#pragma once
#include "YunutyEngine.h"

#include "UnitCommand.h"

class Unit;
class yunuGI::IMesh;

class UnitAttackCommand : public UnitCommand
{
public:
	UnitAttackCommand(Unit* unit, Vector3d targetPos, Vector3d prevPos);
	virtual ~UnitAttackCommand();

public:
	virtual void Execute() override;
	virtual void ShowPreviewMesh() override;
	virtual void HidePreviewMesh() override;

private:
	yunutyEngine::graphics::StaticMeshRenderer* renderer;
};

