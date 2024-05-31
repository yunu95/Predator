#pragma once
#include <memory>

class Reference
{
public:
    struct Guard {};
    std::shared_ptr<Guard> Acquire()
    {
        if (impl.expired())
        {
            auto shared = std::make_shared<Guard>();
            impl = shared;
            return shared;
        }
        return impl.lock();
    }

    bool BeingReferenced() const
    {
        return !impl.expired();
    }
    bool operator()() const
    {
        return BeingReferenced();
    }
private:
    std::weak_ptr<Guard> impl;
};
