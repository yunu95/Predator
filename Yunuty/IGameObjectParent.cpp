#include "IGameObjectParent.h"
#include "GameObject.h"

namespace yunutyEngine
{
    const unordered_set<GameObject*>& yunutyEngine::IGameObjectParent::GetUpdatingChildren()
    {
        return updatingChildren;
    }
    void yunutyEngine::IGameObjectParent::HandleChildUpdateState(GameObject* gameObj)
    {
        if (gameObj->parent == this && gameObj->DeservesUpdate())
            updatingChildren.insert(gameObj);
        else
            updatingChildren.erase(gameObj);
        if (gameObj->parentGameObject != nullptr &&
            gameObj->DeservesUpdate() != gameObj->parentGameObject->DeservesUpdate())
            gameObj->parentGameObject->parent->HandleChildUpdateState(gameObj->parentGameObject);
    }
}