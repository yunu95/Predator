#include "Trigger_Region.h"
#include "RegionData.h"
#include "PlaytimeRegion.h"

#include "Application.h"
#include "EditorLayer.h"
#include "Panel_Palette.h"
#include "RegionPalette.h"

#include "EditorPopupManager.h"

namespace application
{
	Trigger_EnterRegion::~Trigger_EnterRegion()
	{
		if (targetRegion)
		{
			targetRegion->RemoveObserver(this);
		}
	}

	void Trigger_EnterRegion::LinkCallback()
	{
		// Trigger_EnterRegion은 특정 지역에 플레이어 캐릭터가 진입할 시 호출됩니다.
		// LinkCallback 함수가 호출될 때 regionData는 이미 playtimeRegion을 생성한 상태여야 합니다.
		assert(targetRegion && targetRegion->playtimeRegion);
		targetRegion->playtimeRegion->OnEnter.push_back([=]() { PullTrigger(); });
	}

	void Trigger_EnterRegion::SetRegion(editor::RegionData* region)
	{
		if (targetRegion)
		{
			targetRegion->RemoveObserver(this);
		}

		targetRegion = region;
		if (region)
		{
			region->RegisterObserver(this);
		}
	}

	bool Trigger_EnterRegion::IsValid()
	{
		return (targetRegion != nullptr) ? true : false;
	}

	void Trigger_EnterRegion::ProcessObervationEvent(ObservationTarget* target, ObservationEvent event)
	{
		switch (event)
		{
			case application::ObservationEvent::Destroy:
			{
				if (targetRegion == static_cast<editor::RegionData*>(target))
				{
					targetRegion = nullptr;
				}
				break;
			}
			default:
				break;
		}
	}

	void Trigger_EnterRegion::ImGui_DrawDataPopup(Trigger_EnterRegion* data)
	{
		if (ImGui::MenuItem("SetEnterRegion"))
		{
			editor::EditorLayer::SetInputControl(false);
			editor::imgui::ShowMessageBox("SetEnterRegion", [data]()
				{
					auto& pp = editor::PalettePanel::GetSingletonInstance();
					auto& rp = editor::palette::RegionPalette::SingleInstance();

					editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

					ImGui::Separator();

					ImGui::SetNextItemWidth(-1);
					if (data->targetRegion)
					{
						ImGui::Text(yutility::GetString(data->targetRegion->pod.name).c_str());
						pp.ChangeTab("Region");
						rp.Reset();
						rp.SelectRegion(data->targetRegion);
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
		data["targetRegion"] = targetRegion ? UUID_To_String(targetRegion->GetUUID()) : "nullptr";
		return true;
	}

	bool Trigger_EnterRegion::PreDecoding(const json& data)
	{
		return true;
	}

	bool Trigger_EnterRegion::PostDecoding(const json& data)
	{
		SetRegion(UUIDManager::GetSingletonInstance().GetPointerFromUUID<editor::RegionData*>(String_To_UUID(data["targetRegion"])));
		return true;
	}

	Trigger_LeaveRegion::~Trigger_LeaveRegion()
	{
		if (targetRegion)
		{
			targetRegion->RemoveObserver(this);
		}
	}

	void Trigger_LeaveRegion::LinkCallback()
	{
		// Trigger_LeaveRegion은 특정 지역에 플레이어 캐릭터가 이탈할 시 호출됩니다.
		// LinkCallback 함수가 호출될 때 regionData는 이미 playtimeRegion을 생성한 상태여야 합니다.
		assert(targetRegion && targetRegion->playtimeRegion);
		targetRegion->playtimeRegion->OnLeave.push_back([=]() { PullTrigger(); });
	}

	void Trigger_LeaveRegion::SetRegion(editor::RegionData* region)
	{
		if (targetRegion)
		{
			targetRegion->RemoveObserver(this);
		}

		targetRegion = region;
		if (region)
		{
			region->RegisterObserver(this);
		}
	}

	bool Trigger_LeaveRegion::IsValid()
	{
		return (targetRegion != nullptr) ? true : false;
	}

	void Trigger_LeaveRegion::ProcessObervationEvent(ObservationTarget* target, ObservationEvent event)
	{
		switch (event)
		{
			case application::ObservationEvent::Destroy:
			{
				if (targetRegion == static_cast<editor::RegionData*>(target))
				{
					targetRegion = nullptr;
				}
				break;
			}
			default:
				break;
		}
	}

	void Trigger_LeaveRegion::ImGui_DrawDataPopup(Trigger_LeaveRegion* data)
	{
		if (ImGui::MenuItem("SetLeaveRegion"))
		{
			editor::EditorLayer::SetInputControl(false);
			editor::imgui::ShowMessageBox("SetLeaveRegion", [data]()
				{
					auto& pp = editor::PalettePanel::GetSingletonInstance();
					auto& rp = editor::palette::RegionPalette::SingleInstance();

					editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

					ImGui::Separator();

					ImGui::SetNextItemWidth(-1);

					if (data->targetRegion)
					{
						ImGui::Text(yutility::GetString(data->targetRegion->pod.name).c_str());
						pp.ChangeTab("Region");
						rp.Reset();
						rp.SelectRegion(data->targetRegion);
					}
					else
					{
						ImGui::Text("------");
					}

					ImGui::Separator();

					if (ImGui::Button("Edit"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetLeaveRegion");
						editor::EditorLayer::SetInputControl(true);
						editor::EditorPopupManager::GetSingletonInstance().PushReturnPopup<Trigger_LeaveRegion>("SetLeaveRegion", data);
					}
					ImGui::SameLine();

					if (ImGui::Button("Cancel"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetLeaveRegion");
						editor::EditorLayer::SetInputControl(true);
					}
				}, 300);
		}
	}

	bool Trigger_LeaveRegion::PreEncoding(json& data) const
	{
		return true;
	}

	bool Trigger_LeaveRegion::PostEncoding(json& data) const
	{
		data["targetRegion"] = targetRegion ? UUID_To_String(targetRegion->GetUUID()) : "nullptr";
		return true;
	}

	bool Trigger_LeaveRegion::PreDecoding(const json& data)
	{
		return true;
	}

	bool Trigger_LeaveRegion::PostDecoding(const json& data)
	{
		SetRegion(UUIDManager::GetSingletonInstance().GetPointerFromUUID<editor::RegionData*>(String_To_UUID(data["targetRegion"])));
		return true;
	}
}