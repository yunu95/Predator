#pragma once
#include <vector>
#include <functional>

// 델리게이트는 콜백을 모은 후 한번에 실행한다.
class Delegate
{
public:
    void AddCallback(std::function<void()> callback)
    {
        callbacks.push_back(callback);
    }
    void AddVolatileCallback(std::function<void()> callback)
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
private:
    std::vector<std::function<void()>> callbacks;
    std::vector<std::function<void()>> volatileCallbacks;
};
