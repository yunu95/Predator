#pragma once
#include "YunutyEngine.h"
#include "Unit.h"
#include "ContentsObservee.h"

class DummyComponent : public Component, public ContentsObservee
{
public:
	Unit* m_pairUnit;
	virtual void PlayFunction() override;
	virtual void StopFunction() override;
};

void DummyComponent::PlayFunction()
{

}

void DummyComponent::StopFunction()
{
	if (!GetGameObject()->GetComponentWeakPtr<DummyComponent>().expired())
	{
		yunutyEngine::Scene::getCurrentScene()->DestroyGameObject(GetGameObject());
	}
}
