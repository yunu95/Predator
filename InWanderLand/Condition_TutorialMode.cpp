#include "Condition_TutorialMode.h"

#include "TutorialManager.h"

namespace application
{
    bool Condition_TutorialModeOn::IsConditionMet()
    {
        return TutorialManager::Instance().IsTutorialMode();    
    }

    bool Condition_TutorialModeOn::PreEncoding(json& data) const
    {
        return true;
    }

    bool Condition_TutorialModeOn::PostEncoding(json& data) const
    {
        return true;
    }

    bool Condition_TutorialModeOn::PreDecoding(const json& data)
    {
        return true;
    }

    bool Condition_TutorialModeOn::PostDecoding(const json& data)
    {
        return true;
    }

    void Condition_TutorialModeOn::ImGui_DrawDataPopup(Condition_TutorialModeOn* data)
    {

    }

    bool Condition_TutorialModeOff::IsConditionMet()
    {
        return !TutorialManager::Instance().IsTutorialMode();
    }

    bool Condition_TutorialModeOff::PreEncoding(json& data) const
    {
        return true;
    }

    bool Condition_TutorialModeOff::PostEncoding(json& data) const
    {
        return true;
    }

    bool Condition_TutorialModeOff::PreDecoding(const json& data)
    {
        return true;
    }

    bool Condition_TutorialModeOff::PostDecoding(const json& data)
    {
        return true;
    }

    void Condition_TutorialModeOff::ImGui_DrawDataPopup(Condition_TutorialModeOff* data)
    {

    }
}
