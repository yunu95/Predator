#include "Trigger_Wave.h"
#include "WaveData.h"
#include "PlaytimeWave.h"

#include "Application.h"
#include "EditorLayer.h"

#include "EditorPopupManager.h"

namespace application
{
	Trigger_StartWave::~Trigger_StartWave()
	{
		if (targetWave)
		{
			targetWave->RemoveObserver(this);
		}
	}

	void Trigger_StartWave::LinkCallback()
	{
		assert(targetWave && targetWave->playtimeWave);
		targetWave->playtimeWave->waveStartCallbackMap[waveIdx].push_back([=]() { PullTrigger(); });
	}

	void Trigger_StartWave::SetWave(editor::WaveData* wave)
	{
		if (targetWave)
		{
			targetWave->RemoveObserver(this);
		}

		targetWave = wave;
		if (wave)
		{
			wave->RegisterObserver(this);
		}
	}

	void Trigger_StartWave::SetWaveIndex(unsigned int waveIdx)
	{
		this->waveIdx = waveIdx;
	}

	bool Trigger_StartWave::IsValid()
	{
		return (targetWave != nullptr) ? true : false;
	}

	void Trigger_StartWave::ProcessObervationEvent(ObservationTarget* target, ObservationEvent event)
	{
		switch (event)
		{
			case application::ObservationEvent::Destroy:
			{
				if (targetWave == static_cast<editor::WaveData*>(target))
				{
					targetWave = nullptr;
				}
				break;
			}
			default:
				break;
		}
	}

	void Trigger_StartWave::ImGui_DrawDataPopup(Trigger_StartWave* data)
	{
		if (ImGui::MenuItem("SetStartWave"))
		{
			editor::EditorLayer::SetInputControl(false);
			editor::imgui::ShowMessageBox("SetStartWave", [data]()
				{
					editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

					ImGui::Separator();

					ImGui::SetNextItemWidth(-1);
					if (data->targetWave)
					{
						ImGui::Text(yutility::GetString(data->targetWave->pod.name).c_str());
					}
					else
					{
						ImGui::Text("------");
					}

					ImGui::Separator();

					if (ImGui::Button("Edit"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetStartWave");
						editor::EditorLayer::SetInputControl(true);
						editor::EditorPopupManager::GetSingletonInstance().PushReturnPopup<Trigger_StartWave>("SetStartWave", data);
					}
					ImGui::SameLine();

					if (ImGui::Button("Cancel"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetStartWave");
						editor::EditorLayer::SetInputControl(true);
					}
				}, 300);
		}

		if (ImGui::MenuItem("SetWaveIndex(Start)"))
		{
			editor::EditorLayer::SetInputControl(false);
			static int waveIdx = 0;
			waveIdx = data->waveIdx;
			editor::imgui::ShowMessageBox("SetWaveIndex(Start)", [data]()
				{
					editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

					ImGui::Separator();

					ImGui::SetNextItemWidth(-1);
					ImGui::DragInt("##waveIdx(Start)", &waveIdx, 1.0f, 0, 100);

					ImGui::Separator();

					if (ImGui::Button("OK"))
					{
						data->SetWaveIndex(waveIdx);
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetWaveIndex(Start)");
						editor::EditorLayer::SetInputControl(true);
					}
					ImGui::SameLine();

					if (ImGui::Button("Cancel"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetWaveIndex(Start)");
						editor::EditorLayer::SetInputControl(true);
					}
				}, 300);
		}
	}

	bool Trigger_StartWave::PreEncoding(json& data) const
	{
		data["waveIdx"] = waveIdx;
		return true;
	}

	bool Trigger_StartWave::PostEncoding(json& data) const
	{
		data["targetWave"] = targetWave ? UUID_To_String(targetWave->GetUUID()) : "nullptr";
		return true;
	}

	bool Trigger_StartWave::PreDecoding(const json& data)
	{
		waveIdx = data["waveIdx"];
		return true;
	}

	bool Trigger_StartWave::PostDecoding(const json& data)
	{
		SetWave(UUIDManager::GetSingletonInstance().GetPointerFromUUID<editor::WaveData*>(String_To_UUID(data["targetWave"])));
		return true;
	}

	Trigger_EndWave::~Trigger_EndWave()
	{
		if (targetWave)
		{
			targetWave->RemoveObserver(this);
		}
	}

	void Trigger_EndWave::LinkCallback()
	{
		assert(targetWave && targetWave->playtimeWave);
		targetWave->playtimeWave->waveEndCallbackMap[waveIdx].push_back([=]() { PullTrigger(); });
	}

	void Trigger_EndWave::SetWave(editor::WaveData* wave)
	{
		if (targetWave)
		{
			targetWave->RemoveObserver(this);
		}

		targetWave = wave;
		if (wave)
		{
			wave->RegisterObserver(this);
		}
	}

	void Trigger_EndWave::SetWaveIndex(unsigned int waveIdx)
	{
		this->waveIdx = waveIdx;
	}

	bool Trigger_EndWave::IsValid()
	{
		return (targetWave != nullptr) ? true : false;
	}

	void Trigger_EndWave::ProcessObervationEvent(ObservationTarget* target, ObservationEvent event)
	{
		switch (event)
		{
			case application::ObservationEvent::Destroy:
			{
				if (targetWave == static_cast<editor::WaveData*>(target))
				{
					targetWave = nullptr;
				}
				break;
			}
			default:
				break;
		}
	}

	void Trigger_EndWave::ImGui_DrawDataPopup(Trigger_EndWave* data)
	{
		if (ImGui::MenuItem("SetEndWave"))
		{
			editor::EditorLayer::SetInputControl(false);
			editor::imgui::ShowMessageBox("SetEndWave", [data]()
				{
					editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

					ImGui::Separator();

					ImGui::SetNextItemWidth(-1);

					if (data->targetWave)
					{
						ImGui::Text(yutility::GetString(data->targetWave->pod.name).c_str());
					}
					else
					{
						ImGui::Text("------");
					}

					ImGui::Separator();

					if (ImGui::Button("Edit"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetEndWave");
						editor::EditorLayer::SetInputControl(true);
						editor::EditorPopupManager::GetSingletonInstance().PushReturnPopup<Trigger_EndWave>("SetEndWave", data);
					}
					ImGui::SameLine();

					if (ImGui::Button("Cancel"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetEndWave");
						editor::EditorLayer::SetInputControl(true);
					}
				}, 300);
		}

		if (ImGui::MenuItem("SetWaveIndex(End)"))
		{
			editor::EditorLayer::SetInputControl(false);
			static int waveIdx = 0;
			waveIdx = data->waveIdx;
			editor::imgui::ShowMessageBox("SetWaveIndex(End)", [data]()
				{
					editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

					ImGui::Separator();

					ImGui::SetNextItemWidth(-1);
					ImGui::DragInt("##waveIdx(End)", &waveIdx, 1.0f, 0, 100);

					ImGui::Separator();

					if (ImGui::Button("OK"))
					{
						data->SetWaveIndex(waveIdx);
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetWaveIndex(End)");
						editor::EditorLayer::SetInputControl(true);
					}
					ImGui::SameLine();

					if (ImGui::Button("Cancel"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetWaveIndex(End)");
						editor::EditorLayer::SetInputControl(true);
					}
				}, 300);
		}
	}

	bool Trigger_EndWave::PreEncoding(json& data) const
	{
		data["waveIdx"] = waveIdx;
		return true;
	}

	bool Trigger_EndWave::PostEncoding(json& data) const
	{
		data["targetWave"] = targetWave ? UUID_To_String(targetWave->GetUUID()) : "nullptr";
		return true;
	}

	bool Trigger_EndWave::PreDecoding(const json& data)
	{
		waveIdx = data["waveIdx"];
		return true;
	}

	bool Trigger_EndWave::PostDecoding(const json& data)
	{
		SetWave(UUIDManager::GetSingletonInstance().GetPointerFromUUID<editor::WaveData*>(String_To_UUID(data["targetWave"])));
		return true;
	}
}