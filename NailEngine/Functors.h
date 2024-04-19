#pragma once
template<typename T>
struct WeakPtrEqual
{
    bool operator()(const std::weak_ptr<T>& lhs, const std::weak_ptr<T>& rhs) const
    {
        return lhs.lock().get() == rhs.lock().get();
    }
};
template<typename T>
struct WeakPtrHash {
    std::size_t operator()(const std::weak_ptr<T>& wp) const noexcept {
        auto sp = wp.lock();  // Convert weak_ptr to shared_ptr
        if (sp) {
            // If the shared_ptr is valid, hash the pointer it holds
            const auto* ptr = sp.get();
            return std::hash<const T*>()(ptr);
        }
        else {
            // Return a special constant if the weak_ptr is expired
            return std::hash<int>()(0);
        }
    }
};
