#pragma once
#include <concepts>
#include "YunutyEngine.h"

namespace yunutyEngine
{
    // 호출되면 바로 싱글톤 클래스를 자식 클래스타입을 템플릿 파라미터로 받는 템플릿 클래스로 구현한다.
    template<typename T> requires std::derived_from<T, yunutyEngine::Component>
    class SingletonComponent
    {
    public:
        static T& Instance();
        static bool isAvailable() { return yunutyEngine::Scene::getCurrentScene(); }
    protected:
        virtual ~SingletonComponent() = default;
    private:
        static T* instance;
    };
}
template<typename T>
T& yunutyEngine::SingletonComponent<T>::Instance()
{
    if (instance == nullptr)
    {
        instance = yunutyEngine::Scene::getCurrentScene()->AddGameObject()->AddComponent<T>();
    }
    return *instance;
}
template<typename T>
T* yunutyEngine::SingletonComponent<T>::instance = nullptr;
