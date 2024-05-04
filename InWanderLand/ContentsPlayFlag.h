#pragma once
#include <winnt.h>
#include <stdint.h>

enum class ContentsPlayFlag : uint32_t
{
    None = 0,
    ImportUI = 1 << 0,
};
DEFINE_ENUM_FLAG_OPERATORS(ContentsPlayFlag)
