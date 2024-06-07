#pragma once
#include "YunutyEngine.h"


class Unit;
class UnitCommand
{
public:
	// prevPos는 이전 커맨드의 targetPos가 될 것이고, targetPos는 현재 커맨드의 목표지점입니다.
	UnitCommand(Unit* unit, Vector3d targetPos, Vector3d prevPos);
	virtual ~UnitCommand();

public:
	virtual void Execute() = 0;
	virtual void ShowPreviewMesh() = 0;
	virtual void HidePreviewMesh() = 0;

	virtual bool IsDone()  { return this->isDone; }
	virtual void SetIsDone(bool isDone) { this->isDone = isDone; }
	virtual Unit* GetUnit() { return unit; }

protected:
	Unit* unit;
	Vector3d targetPos;
	Vector3d prevPos;
	bool isDone;

};

