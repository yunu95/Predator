#pragma once
#include "YunutyEngine.h"
#include "PermanentObservee.h"

class ResurrectionSystem : public SingletonComponent<ResurrectionSystem>, public PermanentObservee
{
public:
	virtual void Start() override;
};

