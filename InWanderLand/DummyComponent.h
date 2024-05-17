#pragma once
#include "YunutyEngine.h"
#include "Unit.h"
#include "ContentsObservee.h"

class DummyComponent : public ContentsObservee
{
public:
	Unit* m_pairUnit;
	virtual void Start() override;
};

void DummyComponent::Start()
{
}
