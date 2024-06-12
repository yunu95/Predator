#pragma once
#include "YunutyEngine.h"
#include "SkillType.h"
#include "UnitCommand.h"

class Unit;
class yunuGI::IMesh;

class UnitSkillCommand : public UnitCommand
{
public:
	UnitSkillCommand(Unit* unit, Vector3d expectedPos, SkillType::Enum skillType);
	virtual ~UnitSkillCommand();

public:
	virtual void Execute() override;
	virtual void ShowPreviewMesh() override;
	virtual void HidePreviewMesh() override;
	SkillType::Enum GetSkillType() { return skillType; }
private:
	yunutyEngine::graphics::StaticMeshRenderer* renderer;
	SkillType::Enum skillType;
	Vector3d previewStartPos;
	Vector3d skillTargetPos;
};

