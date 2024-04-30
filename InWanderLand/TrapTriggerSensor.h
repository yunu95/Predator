#pragma once
#include "YunutyEngine.h"
#include "ContentsObservee.h"

class Unit;

class TrapTriggerSensor : public Component, public ContentsObservee
{
public:
	Unit* trapUnit;
	virtual void OnTriggerEnter(physics::Collider* collider) override;

	virtual void PlayFunction() override;
	virtual void StopFunction() override;
};

