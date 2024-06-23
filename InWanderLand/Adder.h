#pragma once
#include "Factor.h"
#include "YunutyEngine.h"
#include <algorithm>

namespace factor
{
    template<typename T>
    class Adder : public Factor<T>
    {
    public:
        virtual T IdentityFactor() const;
        virtual operator T()
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
    template<typename T>
    T Adder<T>::IdentityFactor() const { return 0; };
    template<>
    Vector3d Adder<Vector3d>::IdentityFactor() const { return Vector3d::zero; };
}