#pragma once
#include <string>
// 주인공 일행이 지역에 진입했을 때 발생할 수 있는 특수 이벤트를 정의합니다.
// 새로운 연출이 필요하지만 이를 기존의 맵 에디터 기능으로 커버할 수는 없을 때 이 기능을 사용하십시오.
// SpecialEventType을 확장하면 에디터의 지역 팔레트에서 확장된 이벤트 타입을 사용할 수 있습니다.
enum class SpecialEventType
{
    None = 0,
    // None과 End는 유효한 이벤트타입으로 취급되지 않습니다.
    // None과 End 사이에 필요한 이벤트 타입을 추가하십시오.
    Stage1To2Transition,
    Stage2StartRegion,
    End,
};
// SpecialEventType들을 문자열 리스트로 변환합니다. 새로운 이벤트 타입을 추가할 때마다 이 함수를 확장하십시오. 이 함수는 에디터에서 이벤트 타입을 표시할 때 사용됩니다.
constexpr array<std::string, static_cast<size_t>(SpecialEventType::End) + 1> SpecialEventTypeStrings()
{
    return std::array<std::string, static_cast<size_t>(SpecialEventType::End) + 1>
    {
        "None",
        "Stage1To2Transition",
        "Stage2StartRegion",
        "End",
    };
};
// SpecialEventType들을 순회하고 싶을 때 사용됩니다.
constexpr array<SpecialEventType, static_cast<size_t>(SpecialEventType::End) + 1> SpecialEventTypes()
{
    array<SpecialEventType, static_cast<size_t>(SpecialEventType::End) + 1> retArray;
    for (auto i = 0; i < retArray.size(); ++i)
        retArray[i] = static_cast<SpecialEventType>(i);
    return retArray;
}
constexpr std::string SpecialEventTypeToString(SpecialEventType type)
{
    return SpecialEventTypeStrings()[static_cast<size_t>(type)];
}