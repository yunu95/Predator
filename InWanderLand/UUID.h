/// 2023. 11. 20 김상준
/// 에디터에서 사용할 인스턴스들을 관리하기 위하여 고유한 식별자를 사용하기 위한 구조체
/// 해당 구조체는 MS 에서 제공해주는 기본 구조체를 사용함

#pragma once

#include <rpc.h>
#pragma comment(lib, "Rpcrt4.lib")

// We will be able to use the 'UUID' structure.

#include <xhash>

namespace std 
{
    template <>
    class hash<UUID>
    {
    public:
        size_t operator()(const UUID& uuid) const
        {
            return *reinterpret_cast<size_t*>(const_cast<UUID*>(&uuid)->Data4);
        }
    };

    template <>
    class hash<const UUID>
    {
    public:
        size_t operator()(const UUID& uuid) const 
        {
            return *reinterpret_cast<size_t*>(const_cast<UUID*>(&uuid)->Data4);
        }
    };
};
