//#pragma once
//#include "InWanderLand.h"
//
//class wanderYieldInstrcution : public yunutyEngine::coroutine::YieldInstruction
//{
//public:
//    struct promise_type
//    {
//        //wanderYieldInstrcution get_return_object()
//        //{
//            //return wanderYieldInstrcution{ std::coroutine_handle<promise_type>::from_promise(*this) };
//        //}
//        std::suspend_never initial_suspend() { return {}; }
//        std::suspend_never final_suspend() noexcept { return {}; }
//        void return_void() {}
//    };
//    std::coroutine_handle<promise_type> coro_handle;
//
//    ReturnObject(std::coroutine_handle<promise_type> h) : coro_handle(h) {}
//    ~ReturnObject() {
//        if (coro_handle) coro_handle.destroy();
//    }
//private:
//};