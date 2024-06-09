#pragma once

#ifdef YUNUTY_EXPORTS
#define YUNUTY_API __declspec(dllexport)
#else
#define YUNUTY_API __declspec(dllimport)
#endif

#include <vector>
#include <functional>

namespace yunutyEngine
{
    namespace coroutine
    {
        class YieldInstruction;
        struct YUNUTY_API Coroutine
        {
            struct promise_type {
                YieldInstruction* yield = nullptr;
                std::vector<std::function<void()>> destroyCallBack;
                Coroutine get_return_object() {
                    return Coroutine{ std::coroutine_handle<promise_type>::from_promise(*this) };
                }
                std::suspend_always initial_suspend() { return std::suspend_always{}; }
                std::suspend_always final_suspend() noexcept { return std::suspend_always{}; }
                void return_void() {}
                /*void return_value(YieldInstruction& yield) { this->yield = &yield; }
                void return_value(YieldInstruction&& yield) { this->yield = &yield; }*/
                std::suspend_always yield_value(std::suspend_always) { return {}; }
                std::suspend_always yield_value(YieldInstruction& yield) { this->yield = &yield; return {}; }
                std::suspend_always yield_value(YieldInstruction&& yield) { this->yield = &yield; return {}; }
                void unhandled_exception() {  }
            };
            std::coroutine_handle<promise_type> handle;
            explicit Coroutine(std::coroutine_handle<promise_type> h) : handle(h) {}
            ~Coroutine()
            {
                if (handle)
                {
                    for (auto each : handle.promise().destroyCallBack)
                    {
                        each();
                    }
                    handle.destroy();
                }
            }
            Coroutine(Coroutine const&) = delete;
            Coroutine(Coroutine&& other) noexcept
            {
                handle = std::move(other.handle);
                other.handle = nullptr;
            };
            void PushDestroyCallBack(const std::function<void()>& callBack)
            {
                handle.promise().destroyCallBack.push_back(callBack);
            }
            Coroutine& operator=(Coroutine const&) = delete;
            void resume() { handle.resume(); }
            YieldInstruction* GetLastYield() { return handle.promise().yield; };
            bool Done() { return handle.done(); }
        };
    }
}
