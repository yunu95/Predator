#include "Action_WaitForSeconds.h"

#include "EditorLayer.h"
#include "YunutyEngine.h"

namespace application
{
	Action_WaitForSeconds::Action_WaitForSeconds(float time)
		: waitTime(time)
	{

	}

	CoroutineObject<void> Action_WaitForSeconds::DoAction()
	{
		for (float deltaTime = yunutyEngine::Time::GetDeltaTime(); deltaTime < waitTime;)
		{
			co_await std::suspend_always();
			deltaTime += yunutyEngine::Time::GetDeltaTime();
		}

		co_return;
	}

	void Action_WaitForSeconds::SetWaitTime(float time)
	{
		waitTime = time;
	}

	void Action_WaitForSeconds::ImGui_DrawDataPopup(Action_WaitForSeconds* data)
	{
		if (ImGui::MenuItem("SetWaitTime(Scaled)"))
		{
			editor::EditorLayer::SetInputControl(false);
			static float waitTime = 0;
			waitTime = data->waitTime;
			editor::imgui::ShowMessageBox("SetWaitTime(Scaled)", [data]()
				{
					editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

					ImGui::Separator();

					ImGui::SetNextItemWidth(-1);
					ImGui::DragFloat("##WaitTime", &waitTime);

					ImGui::Separator();

					if (ImGui::Button("OK"))
					{
						data->SetWaitTime(waitTime);
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetWaitTime(Scaled)");
						editor::EditorLayer::SetInputControl(true);
					}
					ImGui::SameLine();

					if (ImGui::Button("Cancel"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetWaitTime(Scaled)");
						editor::EditorLayer::SetInputControl(true);
					}
				}, 300);
		}
	}

	bool Action_WaitForSeconds::PreEncoding(json& data) const
	{
		data["waitTime"] = waitTime;
		return true;
	}

	bool Action_WaitForSeconds::PostEncoding(json& data) const
	{
		return true;
	}

	bool Action_WaitForSeconds::PreDecoding(const json& data)
	{
		waitTime = data["waitTime"];
		return true;
	}

	bool Action_WaitForSeconds::PostDecoding(const json& data)
	{
		return true;
	}

	Action_WaitForRealSeconds::Action_WaitForRealSeconds(float time)
		: waitTime(time)
	{

	}

	CoroutineObject<void> Action_WaitForRealSeconds::DoAction()
	{
		for (float deltaTime = yunutyEngine::Time::GetDeltaTimeUnscaled(); deltaTime < waitTime;)
		{
			co_await std::suspend_always();
			deltaTime += yunutyEngine::Time::GetDeltaTimeUnscaled();
		}

		co_return;
	}

	void Action_WaitForRealSeconds::SetWaitTime(float time)
	{
		waitTime = time;
	}

	void Action_WaitForRealSeconds::ImGui_DrawDataPopup(Action_WaitForRealSeconds* data)
	{
		if (ImGui::MenuItem("SetWaitTime(Real)"))
		{
			editor::EditorLayer::SetInputControl(false);
			static float waitTime = 0;
			waitTime = data->waitTime;
			editor::imgui::ShowMessageBox("SetWaitTime(Real)", [data]()
				{
					editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

					ImGui::Separator();

					ImGui::SetNextItemWidth(-1);
					ImGui::DragFloat("##WaitTime", &waitTime);

					ImGui::Separator();

					if (ImGui::Button("OK"))
					{
						data->SetWaitTime(waitTime);
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetWaitTime(Real)");
						editor::EditorLayer::SetInputControl(true);
					}
					ImGui::SameLine();

					if (ImGui::Button("Cancel"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetWaitTime(Real)");
						editor::EditorLayer::SetInputControl(true);
					}
				}, 300);
		}
	}

	bool Action_WaitForRealSeconds::PreEncoding(json& data) const
	{
		data["waitTime"] = waitTime;
		return true;
	}

	bool Action_WaitForRealSeconds::PostEncoding(json& data) const
	{
		return true;
	}

	bool Action_WaitForRealSeconds::PreDecoding(const json& data)
	{
		waitTime = data["waitTime"];
		return true;
	}

	bool Action_WaitForRealSeconds::PostDecoding(const json& data)
	{
		return true;
	}
}