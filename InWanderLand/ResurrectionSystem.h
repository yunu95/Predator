#pragma once
#include "YunutyEngine.h"
#include "PermanentObservee.h"

class ResurrectionSystem : public Component, public SingletonComponent<ResurrectionSystem>, public PermanentObservee
{
public:
	virtual void Start() override;

	virtual void PlayFunction() override;
	virtual void StopFunction() override;
};

