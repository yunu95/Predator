#pragma once
#include "YunutyEngine.h"
#include "SkillType.h"
#include "UnitCommand.h"

class Unit;
class yunuGI::IMesh;

class UnitSkillCommand : public UnitCommand
{
public:
	UnitSkillCommand(Unit* unit, Vector3d expectedPos, SkillType skillType);
	virtual ~UnitSkillCommand();

public:
	virtual void Execute() override;
	virtual void ShowPreviewMesh() override;
	virtual void HidePreviewMesh() override;

private:
	yunutyEngine::graphics::StaticMeshRenderer* renderer;
	SkillType skillType;
};

