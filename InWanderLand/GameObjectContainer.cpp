#include "GameObjectContainer.h"

void GameObjectContainer::RegisterObject(GameObject* p_obj)
{
	objectVector.push_back(p_obj);
}

void GameObjectContainer::ClearVector()
{
	for (auto& e : objectVector)
	{
		yunutyEngine::Scene::getCurrentScene()->DestroyGameObject(e);
	}
}
