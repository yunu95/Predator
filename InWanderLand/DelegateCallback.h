#pragma once
#include <vector>
#include <functional>

// 델리게이트는 콜백을 모은 후 한번에 실행한다.
template <typename T>
class DelegateCallback
{
public:
    void AddCallback(std::function<T> callback)
    {
        callbacks.push_back(callback);
    }
    void AddVolatileCallback(std::function<T> callback) 
    {
        volatileCallbacks.push_back(callback);
    }
    void operator()()
    {
        for (auto& callback : callbacks)
            callback();
        for (auto& callback : volatileCallbacks)
            callback();
        volatileCallbacks.clear();
    }
    void Clear()
    {
        callbacks.clear();
        volatileCallbacks.clear();
    }
private:
    std::vector<std::function<T>> callbacks;
    std::vector<std::function<T>> volatileCallbacks;
};
