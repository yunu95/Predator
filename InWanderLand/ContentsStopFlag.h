#pragma once
#include <winnt.h>
#include <stdint.h>

enum class ContentsStopFlag : uint32_t
{
    None = 0,
    ClearUI = 1 << 0,
};
DEFINE_ENUM_FLAG_OPERATORS(ContentsStopFlag)
