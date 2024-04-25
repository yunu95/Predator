#include "Condition_CinematicMode.h"

#include "CinematicManager.h"

namespace application
{
	bool Condition_CinematicModeOn::IsConditionMet()
	{
		return CinematicManager::Instance().IsCinematicMode();
	}

	bool Condition_CinematicModeOn::PreEncoding(json& data) const
	{
		return true;
	}

	bool Condition_CinematicModeOn::PostEncoding(json& data) const
	{
		return true;
	}

	bool Condition_CinematicModeOn::PreDecoding(const json& data)
	{
		return true;
	}

	bool Condition_CinematicModeOn::PostDecoding(const json& data)
	{
		return true;
	}

	void Condition_CinematicModeOn::ImGui_DrawDataPopup(Condition_CinematicModeOn* data)
	{

	}

	bool Condition_CinematicModeOff::IsConditionMet()
	{
		return !CinematicManager::Instance().IsCinematicMode();
	}

	bool Condition_CinematicModeOff::PreEncoding(json& data) const
	{
		return true;
	}

	bool Condition_CinematicModeOff::PostEncoding(json& data) const
	{
		return true;
	}

	bool Condition_CinematicModeOff::PreDecoding(const json& data)
	{
		return true;
	}

	bool Condition_CinematicModeOff::PostDecoding(const json& data)
	{
		return true;
	}

	void Condition_CinematicModeOff::ImGui_DrawDataPopup(Condition_CinematicModeOff* data)
	{

	}
}
