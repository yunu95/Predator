#pragma once
#include "YunutyEngine.h"
#include "Unit.h"
#include "ContentsObservee.h"

class DummyComponent : public Component, public ContentsObservee
{
public:
	Unit* m_pairUnit;
	virtual void Start() override;
	virtual Component* GetComponent() override { return this; }
};

void DummyComponent::Start()
{
}
