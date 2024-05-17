#pragma once
#include "YunutyEngine.h"
#include "ContentsObservee.h"
#include "ChessTrapSkillSystem.h"

class TrapTriggerSensor : public Component, public ContentsObservee
{
public:
	ChessTrapSkillSystem* trapChessSkillSystem;
	virtual void OnTriggerEnter(physics::Collider* collider) override;
	virtual Component* GetComponent() override { return this; }
};

