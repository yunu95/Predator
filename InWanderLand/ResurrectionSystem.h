#pragma once
#include "YunutyEngine.h"
#include "ContentsObservee.h"

class ResurrectionSystem : public Component, public SingletonComponent<ResurrectionSystem>, public ContentsObservee
{
public:
	virtual void Start() override;

	virtual void PlayFunction() override;
	virtual void StopFunction() override;
};

