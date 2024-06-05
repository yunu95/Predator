#pragma once
#include "YunutyEngine.h"


class Unit;
class UnitCommand
{
public:
	UnitCommand(Unit* unit, Vector3d finalPos, Vector3d targetPos);
	virtual ~UnitCommand();

	bool IsDone()  { return this->isDone; }
	void SetIsDone(bool isDone) { this->isDone = isDone; }
	virtual void Execute() = 0;
	virtual void ShowPreviewMesh() = 0;
	virtual void HidePreviewMesh() = 0;

protected:
	Unit* unit;
	Vector3d finalPos;
	Vector3d targetPos;
	bool isDone;

};

