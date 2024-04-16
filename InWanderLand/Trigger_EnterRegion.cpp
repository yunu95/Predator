#include "Trigger_EnterRegion.h"
#include "RegionData.h"
#include "PlaytimeRegion.h"

#include "Application.h"
#include "EditorLayer.h"

#include "EditorPopupManager.h"

namespace application
{
	void Trigger_EnterRegion::LinkCallback()
	{
		// Trigger_EnterRegion은 특정 지역에 플레이어 캐릭터가 진입할 시 호출됩니다.
		// LinkCallback 함수가 호출될 때 regionData는 이미 playtimeRegion을 생성한 상태여야 합니다.
		assert(enteringRegion && enteringRegion->playtimeRegion);
		enteringRegion->playtimeRegion->OnEnter.push_back([=]() { PullTrigger(); });
	}

	void Trigger_EnterRegion::SetRegion(editor::RegionData* region)
	{
		enteringRegion = region;
	}

	void Trigger_EnterRegion::ImGui_DrawDataPopup(Trigger_EnterRegion* data)
	{
		if (ImGui::MenuItem("SetEnterRegion"))
		{
			editor::EditorLayer::SetInputControl(false);
			editor::imgui::ShowMessageBox("SetEnterRegion", [data]()
				{
					editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

					ImGui::Separator();

					ImGui::SetNextItemWidth(-1);
					if (data->enteringRegion)
					{
						ImGui::Text(yutility::GetString(data->enteringRegion->pod.name).c_str());
					}
					else
					{
						ImGui::Text("------");
					}

					ImGui::Separator();

					if (ImGui::Button("Edit"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetEnterRegion");
						editor::EditorLayer::SetInputControl(true);
						editor::EditorPopupManager::GetSingletonInstance().PushReturnPopup<Trigger_EnterRegion>("SetEnterRegion", data);
					}
					ImGui::SameLine();

					if (ImGui::Button("Cancel"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetEnterRegion");
						editor::EditorLayer::SetInputControl(true);
					}
				}, 300);
		}
	}

	bool Trigger_EnterRegion::PreEncoding(json& data) const
	{
		return true;
	}

	bool Trigger_EnterRegion::PostEncoding(json& data) const
	{
		data["enteringRegion"] = enteringRegion ? UUID_To_String(enteringRegion->GetUUID()) : "nullptr";
		return true;
	}

	bool Trigger_EnterRegion::PreDecoding(const json& data)
	{
		return true;
	}

	bool Trigger_EnterRegion::PostDecoding(const json& data)
	{
		enteringRegion = UUIDManager::GetSingletonInstance().GetPointerFromUUID<editor::RegionData*>(String_To_UUID(data["enteringRegion"]));
		return true;
	}
}