#pragma once
#include <vector>
#include <functional>

class VolatileCallbacks :public std::vector<std::function<void()>>
{
public:
    void operator()()
    {
        for (auto& callback : *this)
        {
            callback();
        }
        clear();
    }
};
