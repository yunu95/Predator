#pragma once

namespace yunutyEngine
{
    // 싱글톤 클래스를 자식 클래스타입을 템플릿 파라미터로 받는 템플릿 클래스로 구현한다.
    template<typename T>
    class SingletonClass
    {
    public:
        static T& SingleInstance();
        SingletonClass(const SingletonClass&) = delete;
        SingletonClass& operator=(const SingletonClass&) = delete;
    protected:
        SingletonClass() = default;
        virtual ~SingletonClass() = default;
    private:
        static T* instance;
    };
}
template<typename T>
T& yunutyEngine::SingletonClass<T>::SingleInstance()
{
    if (instance == nullptr)
        instance = new T();
    return *instance;
}
template<typename T>
T* yunutyEngine::SingletonClass<T>::instance = nullptr;
