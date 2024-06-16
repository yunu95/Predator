#include "Condition_TacticMode.h"

#include "TacticModeSystem.h"

namespace application
{
    bool Condition_TacticModeOn::IsConditionMet()
    {
        return TacticModeSystem::Instance().IsOperation();
    }

    bool Condition_TacticModeOn::PreEncoding(json& data) const
    {
        return true;
    }

    bool Condition_TacticModeOn::PostEncoding(json& data) const
    {
        return true;
    }

    bool Condition_TacticModeOn::PreDecoding(const json& data)
    {
        return true;
    }

    bool Condition_TacticModeOn::PostDecoding(const json& data)
    {
        return true;
    }

    void Condition_TacticModeOn::ImGui_DrawDataPopup(Condition_TacticModeOn* data)
    {

    }

    bool Condition_TacticModeOff::IsConditionMet()
    {
        return !TacticModeSystem::Instance().IsOperation();
    }

    bool Condition_TacticModeOff::PreEncoding(json& data) const
    {
        return true;
    }

    bool Condition_TacticModeOff::PostEncoding(json& data) const
    {
        return true;
    }

    bool Condition_TacticModeOff::PreDecoding(const json& data)
    {
        return true;
    }

    bool Condition_TacticModeOff::PostDecoding(const json& data)
    {
        return true;
    }

    void Condition_TacticModeOff::ImGui_DrawDataPopup(Condition_TacticModeOff* data)
    {

    }
}
