#include "Action_Script.h"

#include "InWanderLand.h"
#include "Application.h"
#include "EditorLayer.h"
#include "Script.h"
#include "ScriptSystem.h"

#include "EditorPopupManager.h"

#include "YunutyEngine.h"

namespace application
{
	Action_PullScriptTrigger::~Action_PullScriptTrigger()
	{
		if (targetScript)
		{
			targetScript->RemoveObserver(this);
		}
	}

	CoroutineObject<void> Action_PullScriptTrigger::DoAction()
	{
		targetScript->PullScriptTrigger();
		co_return;
	}

	bool Action_PullScriptTrigger::IsValid()
	{
		return (targetScript == nullptr) ? false : true;
	}

	void Action_PullScriptTrigger::SetTargetScript(Script* script)
	{
		if (targetScript)
		{
			targetScript->RemoveObserver(this);
		}

		this->targetScript = script;
		if (script)
		{
			script->RegisterObserver(this);
		}
	}

	void Action_PullScriptTrigger::ProcessObervationEvent(ObservationTarget* target, ObservationEvent event)
	{
		switch (event)
		{
			case application::ObservationEvent::Destroy:
			{
				if (targetScript == static_cast<Script*>(target))
				{
					targetScript = nullptr;
				}
				break;
			}
			default:
				break;
		}
	}


	void Action_PullScriptTrigger::ImGui_DrawDataPopup(Action_PullScriptTrigger* data)
	{
		if (ImGui::MenuItem("SetTargetScript(PullTrigger)"))
		{
			editor::EditorLayer::SetInputControl(false);
			static int currentIdx = 0;
			static Script* script = nullptr;
			script = data->targetScript;
			editor::imgui::ShowMessageBox("SetTargetScript(PullTrigger)", [data]()
				{
					editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

					ImGui::Separator();

					ImGui::SetNextItemWidth(-1);

					static std::map<Script*, std::string, ScriptPredicate> nameMap;
					static std::vector<const char*> nameList = std::vector<const char*>();

					nameMap.clear();
					nameMap[nullptr] = "None";
					for (auto& each : ScriptSystem::Instance().GetScriptList())
					{
						nameMap[each] = each->name;
					}

					nameList.clear();
					for (auto& [each, name] : nameMap)
					{
						nameList.push_back(name.c_str());
					}

					currentIdx = 0;
					for (auto i = nameMap.begin(); i != nameMap.end(); i++)
					{
						if (script == i->first)
						{
							break;
						}
						currentIdx++;
					}

					if (ImGui::BeginCombo("##Script Combo", nameList[currentIdx]))
					{
						auto itr = nameMap.begin();
						for (int i = 0; i < nameList.size(); i++)
						{
							const bool is_selected = (nameList[currentIdx] == nameList[i]);
							if (ImGui::Selectable(nameList[i], is_selected))
							{
								script = itr->first;
							}

							if (is_selected)
								ImGui::SetItemDefaultFocus();

							itr++;
						}
						ImGui::EndCombo();
					}

					ImGui::Separator();

					if (ImGui::Button("OK"))
					{
						data->SetTargetScript(script);
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetTargetScript(PullTrigger)");
						editor::EditorLayer::SetInputControl(true);
					}
					ImGui::SameLine();

					if (ImGui::Button("Cancel"))
					{
						ImGui::CloseCurrentPopup();
						editor::imgui::CloseMessageBox("SetTargetScript(PullTrigger)");
						editor::EditorLayer::SetInputControl(true);
					}
				}, 300);
		}
	}

	bool Action_PullScriptTrigger::PreEncoding(json& data) const
	{
		return true;
	}

	bool Action_PullScriptTrigger::PostEncoding(json& data) const
	{
		data["targetUnit"] = targetScript ? UUID_To_String(targetScript->GetUUID()) : "nullptr";
		return true;
	}

	bool Action_PullScriptTrigger::PreDecoding(const json& data)
	{
		return true;
	}

	bool Action_PullScriptTrigger::PostDecoding(const json& data)
	{
		SetTargetScript(UUIDManager::GetSingletonInstance().GetPointerFromUUID<Script*>(String_To_UUID(data["targetUnit"])));
		return true;
	}
}
