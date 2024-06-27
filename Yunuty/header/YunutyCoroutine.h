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
    class Component;
    class YunutyCycle;
    namespace coroutine
    {
        class YieldInstruction;
        struct YUNUTY_API Coroutine
        {
            struct Coroutine_Handler;
            struct promise_type {
                YieldInstruction* yield = nullptr;
                std::vector<std::function<void()>> destroyCallBack;
                Coroutine get_return_object()
                {
                    return Coroutine(std::make_shared<Coroutine_Handler>(std::coroutine_handle<promise_type>::from_promise(*this)));
                }
                std::suspend_always initial_suspend() { return std::suspend_always{}; }
                std::suspend_always final_suspend() noexcept { return std::suspend_always{}; }
                void return_void() {}
                /*void return_value(YieldInstruction& yield) { this->yield = &yield; }
                void return_value(YieldInstruction&& yield) { this->yield = &yield; }*/
                std::suspend_always yield_value(std::suspend_always) { return {}; }
                std::suspend_always yield_value(YieldInstruction& yield) { this->yield = &yield; return {}; }
                std::suspend_always yield_value(YieldInstruction&& yield) { this->yield = &yield; return {}; }
                void unhandled_exception() 
                {
                    std::rethrow_exception(std::current_exception());
                }
            };
            explicit Coroutine(const std::shared_ptr<Coroutine_Handler>& h) : handle(h) {}
            ~Coroutine()
            {
                if (handle)
                {
                    for (auto each : handle->promise().destroyCallBack)
                    {
                        each();
                    }
                }
            }
            Coroutine(Coroutine const&) = delete;
            Coroutine(Coroutine&& other) noexcept
            {
                deathWish = other.deathWish;
                handle = other.handle;
                other.handle.reset();
            };
            void PushDestroyCallBack(const std::function<void()>& callBack)
            {
                handle->promise().destroyCallBack.push_back(callBack);
            }
            Coroutine& operator=(Coroutine const&) = delete;
            void resume() { handle->resume(); }
            YieldInstruction* GetLastYield() { return handle->promise().yield; };
            bool Done() { return handle->done(); }

            class Coroutine_Handler
            {
                friend struct Coroutine;
            public:
                Coroutine_Handler(const std::coroutine_handle<promise_type>& handle)
                    : primitive_handle(handle)
                {

                }

                ~Coroutine_Handler()
                {
                    if (primitive_handle)
                    {
                        primitive_handle.destroy();
                    }
                }

                auto& promise()
                {
                    return primitive_handle.promise();
                }
                
                void resume()
                {
                    primitive_handle.resume();
                }

                bool done()
                {
                    return primitive_handle.done();
                }

            private:
                std::coroutine_handle<promise_type> primitive_handle;
            };

            std::shared_ptr<Coroutine_Handler> handle;
        private:
            bool deathWish = false;
            friend yunutyEngine::Component;
            friend yunutyEngine::YunutyCycle;
        };
    }
}
