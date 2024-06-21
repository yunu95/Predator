#include "Factor.h"
#include <algorithm>

namespace factor
{
    template<typename T>
    class Adder : public Factor<T>
    {
    public:
        virtual T IdentityFactor() const { return 0; };
        virtual operator T() const
        {
            T identity = IdentityFactor();
            std::erase_if(*this, [](const std::weak_ptr<T>& factor) { return factor.expired(); });
            std::for_each(std::vector<std::weak_ptr<T>>::begin(), std::vector<std::weak_ptr<T>>::end(), [&identity](const std::weak_ptr<T>& factor)
                {
                    if (auto spFactor = factor.lock())
                    {
                        identity += *spFactor;
                    }
                });
            return identity;
        };
    };

}