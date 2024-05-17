#pragma once
#include "YunutyEngine.h"
#include "ContentsObservee.h"
#include "ChessTrapSkillSystem.h"

class TrapTriggerSensor : public ContentsObservee
{
public:
	ChessTrapSkillSystem* trapChessSkillSystem;
	virtual void OnTriggerEnter(physics::Collider* collider) override;
};

