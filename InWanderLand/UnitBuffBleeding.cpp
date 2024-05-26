#include "UnitBuffBleeding.h"

coroutine::Coroutine UnitBuffBleeding::operator()()
{
    auto period = coroutine::ForSeconds{ duration };
    while (period.Tick())
    {
        //owner->damage(inflictor,damag*dt);
        co_await std::suspend_always{};
    }
    co_return;
}
