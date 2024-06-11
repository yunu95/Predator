#pragma once
#include <vector>
#include <memory>

namespace factor
{
    template<typename T>
    class Factor : protected std::vector<std::weak_ptr<T>>
    {
    public:
        std::shared_ptr<T> AcquireFactor()
        {
            auto factor = std::make_shared<T>(IdentityFactor());
            std::vector<std::weak_ptr<T>>::push_back(factor);
            return factor;
        };
        virtual T IdentityFactor()const = 0;
        virtual operator T() = 0;
    };
}
