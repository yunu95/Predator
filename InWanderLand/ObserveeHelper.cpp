#include "ObserveeHelper.h"

void ObserveeHelper::Start()
{
}

void ObserveeHelper::PlayFunction()
{

}

void ObserveeHelper::StopFunction()
{
	if (!GetGameObject()->GetComponentWeakPtr<ObserveeHelper>().expired())
	{
		yunutyEngine::Scene::getCurrentScene()->DestroyGameObject(GetGameObject());
	}
}
