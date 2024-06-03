#pragma once
#include <compare>
#include <memory>

namespace yunutyEngine
{
    // yunutyUtility에는 함수 정의, 혹은 functor만 들어간다.
    namespace yutility
    {
        template<typename T>
        struct GenericEqual {
            bool operator()(const T& lhs, const T& rhs) const {
                // Compare Ts for equality
                return (!memcmp(&(lhs), &(rhs), sizeof(T)));
            }
        };
        template<typename T>
        struct WeakPtrEqual
        {
            bool operator()(const std::weak_ptr<T>& lhs, const std::weak_ptr<T>& rhs) const
            {
                return lhs.lock().get() == rhs.lock().get();
            }
        };
    }
}
