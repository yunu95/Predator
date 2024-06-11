#pragma once
//#include "InWanderLand.h"
#include "YunutyEngine.h"

class WanderCoroutine
{
public:
    struct promise_type
    {
        WanderCoroutine get_return_object()
        {
            return WanderCoroutine{ std::coroutine_handle<promise_type>::from_promise(*this) };
        }
        std::suspend_never initial_suspend() { return {}; }
        std::suspend_never final_suspend() noexcept { return {}; }
        void return_void() {}
    };
    std::coroutine_handle<promise_type> coro_handle;
    //virtual bool ShouldResume() const override {};
    //virtual void Update() override {};
    WanderCoroutine(std::coroutine_handle<promise_type> h) : coro_handle(h) {}
    ~WanderCoroutine() {
        if (coro_handle) coro_handle.destroy();
    }
private:
};