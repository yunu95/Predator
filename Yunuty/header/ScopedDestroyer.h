#pragma once
#include "Component.h"
#include "GameObject.h"
#include "Scene.h"

namespace yunutyEngine
{
    template <class T> requires std::is_base_of_v<Component, T>
    class ScopedDestroyer
    {
    private:
        std::weak_ptr<T> component;
    public:
        ScopedDestroyer(std::weak_ptr<T> component) : component(component) { }
        ~ScopedDestroyer()
        {
            if (component.expired() == false)
            {
                auto comp = component.lock();
                auto obj = comp->GetGameObject();
                obj->GetScene()->DestroyGameObject(obj);
            };
        }
    };
}
