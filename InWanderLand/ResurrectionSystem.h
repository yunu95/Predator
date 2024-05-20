#pragma once
#include "YunutyEngine.h"
#include "PermanentObservee.h"

class ResurrectionSystem : public SingletonComponent<ResurrectionSystem>, public Component, public PermanentObservee
{
public:
	virtual void Start() override;
};

