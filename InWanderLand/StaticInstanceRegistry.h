#pragma once

#include <unordered_set>

template<typename T>
class StaticInstanceRegistry
{
public:
    static const std::unordered_set<T*>& GetInstances() { return instances; }
protected:
    StaticInstanceRegistry()
    {
        instances.insert(static_cast<T*>(this));
    };
    ~StaticInstanceRegistry()
    {
        instances.erase(static_cast<T*>(this));
    }
private:
    static std::unordered_set<T*> instances;
};
template<typename T>
std::unordered_set<T*> StaticInstanceRegistry<T>::instances;
//template<typename T>
//const std::unordered_set<T*>& StaticInstanceRegistry<T>::GetInstances() { return instances; }
