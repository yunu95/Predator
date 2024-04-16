#include "Trigger_RepeatPeriodically.h"

#include "GamePlayTimer.h"
#include "EditorLayer.h"

namespace application
{
	void Trigger_RepeatPeriodically::LinkCallback()
	{
		GamePlayTimer::Instance().repeatCallback[repeatTime].push_back(
			[this]() 
			{
				PullTrigger();
			}
		);
	}

	void Trigger_RepeatPeriodically::SetRepeatTime(float time)
	{
		repeatTime = time;
	}

	void Trigger_RepeatPeriodically::ImGui_DrawDataPopup(Trigger_RepeatPeriodically* data)
	{
		if (ImGui::MenuItem("SetRepeatTime"))
		{
			editor::EditorLayer::SetInputControl(false);
			static float repeatTime = 0;
			repeatTime = data->repeatTime;
			editor::imgui::ShowMessageBox("SetRepeatTime", [data]()
				{
					editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

					ImGui::Separator();

					ImGui::SetNextItemWidth(-1);
					ImGui::DragFloat("##repeatTime", &repeatTime);

					ImGui::Separator();

					if (ImGui::Button("OK"))
					{
						data->SetRepeatTime(repeatTime);
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetRepeatTime");
						editor::EditorLayer::SetInputControl(true);
					}
					ImGui::SameLine();

					if (ImGui::Button("Cancel"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetRepeatTime");
						editor::EditorLayer::SetInputControl(true);
					}
				}, 300);
		}
	}

	bool Trigger_RepeatPeriodically::PreEncoding(json& data) const
	{
		data["repeatTime"] = repeatTime;
		return true;
	}

	bool Trigger_RepeatPeriodically::PostEncoding(json& data) const
	{
		return true;
	}

	bool Trigger_RepeatPeriodically::PreDecoding(const json& data)
	{
		repeatTime = data["repeatTime"];
		return true;
	}

	bool Trigger_RepeatPeriodically::PostDecoding(const json& data)
	{
		return true;
	}

	void Trigger_RepeatPeriodicallyRealTime::LinkCallback()
	{
		GamePlayTimer::Instance().repeatCallbackReal[repeatTime].push_back(
			[this]()
			{
				PullTrigger();
			}
		);
	}

	void Trigger_RepeatPeriodicallyRealTime::SetRepeatTime(float time)
	{
		repeatTime = time;
	}

	void Trigger_RepeatPeriodicallyRealTime::ImGui_DrawDataPopup(Trigger_RepeatPeriodicallyRealTime* data)
	{
		if (ImGui::MenuItem("SetRepeatTime(Real)"))
		{
			editor::EditorLayer::SetInputControl(false);
			static float repeatTime = 0;
			repeatTime = data->repeatTime;
			editor::imgui::ShowMessageBox("SetRepeatTime(Real)", [data]()
				{
					editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

					ImGui::Separator();

					ImGui::SetNextItemWidth(-1);
					ImGui::DragFloat("##repeatTime(Real)", &repeatTime);

					ImGui::Separator();

					if (ImGui::Button("OK"))
					{
						data->SetRepeatTime(repeatTime);
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetRepeatTime(Real)");
						editor::EditorLayer::SetInputControl(true);
					}
					ImGui::SameLine();

					if (ImGui::Button("Cancel"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetRepeatTime(Real)");
						editor::EditorLayer::SetInputControl(true);
					}
				}, 300);
		}
	}

	bool Trigger_RepeatPeriodicallyRealTime::PreEncoding(json& data) const
	{
		data["repeatTime"] = repeatTime;
		return true;
	}

	bool Trigger_RepeatPeriodicallyRealTime::PostEncoding(json& data) const
	{
		return true;
	}

	bool Trigger_RepeatPeriodicallyRealTime::PreDecoding(const json& data)
	{
		repeatTime = data["repeatTime"];
		return true;
	}

	bool Trigger_RepeatPeriodicallyRealTime::PostDecoding(const json& data)
	{
		return true;
	}
}
