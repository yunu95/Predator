#pragma once
#include <iostream>
#include <unordered_set>
#include <vector>

#ifdef YUNUTY_EXPORTS
#define YUNUTY_API __declspec(dllexport)
#else
#define YUNUTY_API __declspec(dllimport)
#endif

using namespace std;
namespace yunutyEngine
{
    class GameObject;
    class Scene;

    class IGameObjectParent
    {
    private:
        virtual unique_ptr<yunutyEngine::GameObject> MoveChild(GameObject* child) = 0;
        virtual void ReceiveChild(remove_reference<unique_ptr<GameObject>>::type&& child) = 0;
        virtual int GetChildIndex(const GameObject* child)const = 0;
        virtual void SetChildIndex(GameObject* child, int index) = 0;
        virtual const unordered_set<GameObject*>& GetUpdatingChildren();
        virtual void HandleChildUpdateState(GameObject* gameObj);
        unordered_set<GameObject*> updatingChildren;
    public:
        virtual const vector<GameObject*>& GetChildren()const = 0;
        friend Component;
        friend GameObject;
        friend Scene;
        friend YunutyCycle;
    };
}
