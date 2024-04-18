#pragma once
#include "YunutyEngine.h"

class Unit;

class TrapTriggerSensor : public Component
{
public:
	Unit* trapUnit;
	virtual void OnTriggerEnter(physics::Collider* collider) override;
};

