#pragma once
#include <vector>
#include <functional>

// 델리게이트는 콜백을 모은 후 한번에 실행한다.
template <typename... Args>
class DelegateCallback
{
public:
    void AddCallback(std::function<void(Args...)> callback)
    {
        callbacks.push_back(callback);
    }
    void AddVolatileCallback(std::function<void(Args...)> callback)
    {
        volatileCallbacks.push_back(callback);
    }
    void operator()(Args... args)
    {
        for (auto& callback : callbacks)
            callback(std::forward<Args>(args)...);
        for (auto& callback : volatileCallbacks)
            callback(std::forward<Args>(args)...);
        volatileCallbacks.clear();
    }
    void Clear()
    {
        callbacks.clear();
        volatileCallbacks.clear();
    }
private:
    std::vector<std::function<void(Args...)>> callbacks;
    std::vector<std::function<void(Args...)>> volatileCallbacks;
};

template <>
class DelegateCallback<void> {
public:
    void AddCallback(std::function<void()> callback) {
        callbacks.push_back(callback);
    }

    void AddVolatileCallback(std::function<void()> callback) {
        volatileCallbacks.push_back(callback);
    }

    void operator()() {
        for (auto& callback : callbacks) {
            callback();
        }
        for (auto& callback : volatileCallbacks) {
            callback();
        }
        volatileCallbacks.clear();
    }

    void Clear() {
        callbacks.clear();
        volatileCallbacks.clear();
    }

private:
    std::vector<std::function<void()>> callbacks;
    std::vector<std::function<void()>> volatileCallbacks;
};
