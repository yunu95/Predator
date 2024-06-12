#pragma once
#include "YunutyEngine.h"


class Unit;
class UnitCommand
{
public:
	enum CommandType
	{
		Move,
		Attack,
		Skill
	};
public:
	// expectedPos는 현재 커맨드가 실행되고 나서 유닛의 위치값을 넣어주어야 합니다.
	UnitCommand(Unit* unit, Vector3d expectedPos);
	virtual ~UnitCommand();

public:
	virtual void Execute() = 0;
	virtual void ShowPreviewMesh() = 0;
	virtual void HidePreviewMesh() = 0;

	virtual bool IsDone()  { return this->isDone; }
	virtual void SetIsDone(bool isDone) { this->isDone = isDone; }
	virtual Unit* GetUnit() { return unit; }
	const Vector3d& GetExpectedPos() { return expectedPos; }
	CommandType GetCommandType() { return this->commandType; }
	float GetCommandCost() { return this->commandCost; }

protected:
	Unit* unit;
	Vector3d expectedPos;
	bool isDone;
	CommandType commandType;
	float commandCost = 0.f;
};

