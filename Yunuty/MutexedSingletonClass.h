#pragma once
#include "MutexedReference.h"

namespace yunutyEngine
{
    // 싱글톤 클래스 중 여러 스레드가 동시에 객체에 접근하면 안되는 클래스에 사용한다.
    template<typename T>
    class MutexedSingletonClass
    {
    public:
        static MutexedReference<T> SingleInstance();
    protected:
        virtual ~MutexedSingletonClass() = default;
    private:
        static std::mutex mtx;
        static T* instance;
    };
}
template<typename T>
yunutyEngine::MutexedReference<T> yunutyEngine::MutexedSingletonClass<T>::SingleInstance()
{
    if (instance == nullptr)
        instance = new T();
    return MutexedReference<T>(std::ref(instance), std::ref(mtx));
}
template<typename T>
T* yunutyEngine::MutexedSingletonClass<T>::instance = nullptr;
