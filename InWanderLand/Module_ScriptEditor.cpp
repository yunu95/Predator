#include "InWanderLand.h"
#include "Module_ScriptEditor.h"

#include "WanderUtils.h"
#include "Application.h"
#include "EditorLayer.h"
#include "EditorInputManager.h"
#include "Script.h"

namespace application
{
	namespace editor
	{
		Module_ScriptEditor::Module_ScriptEditor()
		{

		}

		Module_ScriptEditor::~Module_ScriptEditor()
		{

		}

		void Module_ScriptEditor::Initialize()
		{

		}

		void Module_ScriptEditor::Update(float ts)
		{
			static auto& eim = EditorInputManager::GetSingletonInstance();
			if (eim.IsKeyboardPressed(KeyCode::Delete))
			{
				if (selectedAction)
				{
					selectedScript->EraseAction(selectedAction);
					selectedAction = nullptr;
				}
				else if (selectedTrigger)
				{
					selectedScript->EraseTrigger(selectedTrigger);
					selectedTrigger = nullptr;
				}
				else if (selectedCondition)
				{
					selectedScript->EraseCondition(selectedCondition);
					selectedCondition = nullptr;
				}
				else if (selectedScript)
				{
					ScriptSystem::Instance().EraseScript(selectedScript);
					selectedScript = nullptr;
				}
			}
		}

		void Module_ScriptEditor::GUIProgress()
		{
			CheckActivation();

			ImGui::Begin("Script Editor", &activation);

			/// ImGui 관련 내부 변수 업데이트
			isMouseOver = ImGui::IsWindowHovered();
			isFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);

			/// 실제 패널에 그리는 영역
			DrawLayout();

			ImGui::End();
		}

		void Module_ScriptEditor::Finalize()
		{

		}

		void Module_ScriptEditor::Clear()
		{
			selectedScript = nullptr;
			selectedTrigger = nullptr;
			selectedCondition = nullptr;
			selectedAction = nullptr;
		}

		Script* Module_ScriptEditor::GetSelectedScript()
		{
			return selectedScript;
		}

		void Module_ScriptEditor::DrawLayout()
		{
			// Spacing, 바꾸지 않는다는 전제로 static으로 구현
			static auto spacing = ImGui::GetStyle().ItemSpacing;

			auto region = ImGui::GetContentRegionAvail();

			DrawList(ImVec2(region.x * 0.2 - spacing.x / 2, region.y - spacing.y / 2));
			ImGui::SameLine();
			DrawScriptPanel(ImVec2(region.x * 0.8 - spacing.x / 2, region.y - spacing.y / 2));
		}

		void Module_ScriptEditor::DrawList(const ImVec2& region)
		{
			ImGui::BeginChild("ScriptList", region, true);
			if (ImGui::Button("Create Script", ImVec2(ImGui::GetContentRegionAvail().x, 20)))
			{
				selectedScript = ScriptSystem::Instance().CreateScript();
			}

			imgui::ShiftCursorY(10);

			auto selectedData = GetSelectedScript();

			int idx = 0;
			for (auto each : ScriptSystem::Instance().GetScriptList())
			{
				ImGui::PushID(("Script_Name" + each->name).c_str() + idx);

				if (ImGui::Selectable(each->name.c_str(), selectedData == each))
				{
					if (selectedScript == each)
					{
						selectedScript = nullptr;
					}
					else
					{
						selectedScript = each;
					}
					selectedTrigger = nullptr;
					selectedCondition = nullptr;
					selectedAction = nullptr;
				}
				ImGui::PopID();
				idx++;
			}

			ImGui::EndChild();
		}

		void Module_ScriptEditor::DrawScriptPanel(const ImVec2& region)
		{
			ImGui::BeginChild("SelectedScript", region, true);

			auto selectedData = GetSelectedScript();

			if (selectedData != nullptr)
			{
				DrawScript(selectedData);
			}
			ImGui::EndChild();
		}

		void Module_ScriptEditor::DrawScript(Script* data)
		{
			std::string scriptName = data->name;
			scriptName.reserve(32);
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.5);
			ImGui::InputText("##Script_Name", &scriptName[0], 32);
			wanderUtils::UpdateStringSize(scriptName);
			data->name = scriptName;

			ImGui::SameLine();
			if (ImGui::Button("Add Trigger"))
			{
				ImGui_AddTriggerPopup();
			}

			ImGui::SameLine();
			if (ImGui::Button("Add Condition"))
			{
				ImGui_AddConditionPopup();
			}

			ImGui::SameLine();
			if (ImGui::Button("Add Action"))
			{
				ImGui_AddActionPopup();
			}
			imgui::ShiftCursorY(10);

			if (ImGui::CollapsingHeader("Triggers"))
			{
				for (auto& each : data->triggerList)
				{
					DrawTrigger(each);
				}
			}
			else
			{
				selectedTrigger = nullptr;
			}
			imgui::ShiftCursorY(10);

			if (ImGui::CollapsingHeader("Conditions"))
			{
				for (auto& each : data->conditionList)
				{
					DrawCondition(each);
				}
			}
			else
			{
				selectedCondition = nullptr;
			}
			imgui::ShiftCursorY(10);

			if (ImGui::CollapsingHeader("Actions"))
			{
				for (auto& [key, action] : data->actionList)
				{
					DrawAction(action);
				}
			}
			else
			{
				selectedAction = nullptr;
			}
		}

		void Module_ScriptEditor::ImGui_AddTriggerPopup()
		{
			EditorLayer::SetInputControl(false);
			imgui::ShowMessageBox("Add Trigger", [this]()
				{
					imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

					ImGui::Separator();

					static std::string triggerName = "None";
					static TriggerType triggerType = TriggerType::None;

					ImGui::SetNextItemWidth(-1);
					if (ImGui::BeginCombo("##TriggerListCombo", triggerName.c_str()))
					{
						for (auto& [type, str] : ScriptSystem::triggerList)
						{
							const bool is_selected = (triggerName == str);
							if (ImGui::Selectable(str.c_str(), is_selected))
							{
								triggerName = str;
								triggerType = type;
							}

							if (is_selected)
							{
								ImGui::SetItemDefaultFocus();
							}
						}
						ImGui::EndCombo();
					}

					ImGui::Separator();

					if (ImGui::Button("Add"))
					{
						if (triggerName != "None")
						{
							switch (triggerType)
							{
								case application::TriggerType::GameStart:
								{
									selectedScript->AddTrigger<Trigger_GameStart>();
									break;
								}
								case application::TriggerType::EnterRegion:
								{
									selectedScript->AddTrigger<Trigger_EnterRegion>();
									break;
								}
								case application::TriggerType::LeaveRegion:
								{
									selectedScript->AddTrigger<Trigger_LeaveRegion>();
									break;
								}
								default:
									break;
							}

							triggerName = "None";
							triggerType = TriggerType::None;
							ImGui::CloseCurrentPopup();
							imgui::CloseMessageBox("Add Trigger");
							EditorLayer::SetInputControl(true);
						}
					}
					ImGui::SameLine();

					if (ImGui::Button("Cancel"))
					{
						triggerName = "None";
						triggerType = TriggerType::None;
						ImGui::CloseCurrentPopup();
						imgui::CloseMessageBox("Add Trigger");
						EditorLayer::SetInputControl(true);
					}
				}, 300);
		}

		void Module_ScriptEditor::ImGui_AddConditionPopup()
		{
			EditorLayer::SetInputControl(false);
			imgui::ShowMessageBox("Add Condition", [this]()
				{
					imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

					ImGui::Separator();

					static std::string conditionName = "None";
					static ConditionType conditionType = ConditionType::None;

					ImGui::SetNextItemWidth(-1);
					if (ImGui::BeginCombo("##ConditionListCombo", conditionName.c_str()))
					{
						for (auto& [type, str] : ScriptSystem::conditionList)
						{
							const bool is_selected = (conditionName == str);
							if (ImGui::Selectable(str.c_str(), is_selected))
							{
								conditionName = str;
								conditionType = type;
							}

							if (is_selected)
							{
								ImGui::SetItemDefaultFocus();
							}
						}
						ImGui::EndCombo();
					}

					ImGui::Separator();

					if (ImGui::Button("Add"))
					{
						if (conditionName != "None")
						{
							switch (conditionType)
							{
								default:
									break;
							}

							conditionName = "None";
							conditionType = ConditionType::None;
							ImGui::CloseCurrentPopup();
							imgui::CloseMessageBox("Add Condition");
							EditorLayer::SetInputControl(true);
						}
					}
					ImGui::SameLine();

					if (ImGui::Button("Cancel"))
					{
						conditionName = "None";
						conditionType = ConditionType::None;
						ImGui::CloseCurrentPopup();
						imgui::CloseMessageBox("Add Condition");
						EditorLayer::SetInputControl(true);
					}
				}, 300);
		}

		void Module_ScriptEditor::ImGui_AddActionPopup()
		{
			EditorLayer::SetInputControl(false);
			imgui::ShowMessageBox("Add Action", [this]()
				{
					imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

					ImGui::Separator();

					static std::string actionName = "None";
					static ActionType actionType = ActionType::None;

					ImGui::SetNextItemWidth(-1);
					if (ImGui::BeginCombo("##ActionListCombo", actionName.c_str()))
					{
						for (auto& [type, str] : ScriptSystem::actionList)
						{
							const bool is_selected = (actionName == str);
							if (ImGui::Selectable(str.c_str(), is_selected))
							{
								actionName = str;
								actionType = type;
							}

							if (is_selected)
							{
								ImGui::SetItemDefaultFocus();
							}
						}
						ImGui::EndCombo();
					}

					ImGui::Separator();

					if (ImGui::Button("Add"))
					{
						if (actionName != "None")
						{
							switch (actionType)
							{
								case application::ActionType::WaitForSeconds:
								{
									selectedScript->AddAction<Action_WaitForSeconds>();
									break;
								}
								case application::ActionType::WaitForRealSeconds:
								{
									selectedScript->AddAction<Action_WaitForRealSeconds>();
									break;
								}
								case application::ActionType::CinematicModeChange:
								{
									selectedScript->AddAction<Action_CinematicModeChange>();
									break;
								}
								default:
									break;
							}

							actionName = "None";
							actionType = ActionType::None;
							ImGui::CloseCurrentPopup();
							imgui::CloseMessageBox("Add Action");
							EditorLayer::SetInputControl(true);
						}
					}
					ImGui::SameLine();

					if (ImGui::Button("Cancel"))
					{
						actionName = "None";
						actionType = ActionType::None;
						ImGui::CloseCurrentPopup();
						imgui::CloseMessageBox("Add Action");
						EditorLayer::SetInputControl(true);
					}
				}, 300);
		}

		void Module_ScriptEditor::DrawTrigger(std::shared_ptr<ITrigger> data)
		{
			ImGui::PushID(data.get());
			if (ImGui::Selectable(ScriptSystem::triggerList[data->GetType()].c_str(), data == selectedTrigger))
			{
				if (selectedTrigger == data)
				{
					selectedTrigger = nullptr;
				}
				else
				{
					selectedTrigger = data;
				}
				selectedCondition = nullptr;
				selectedAction = nullptr;
			}

			if ((selectedTrigger == data) && ImGui::IsItemClicked(ImGuiMouseButton_Right))
			{
				ImGui::OpenPopup(ScriptSystem::triggerList[data->GetType()].c_str());
			}

			PopUpDataEdit<ITrigger>(data.get());

			ImGui::PopID();
		}

		void Module_ScriptEditor::DrawCondition(std::shared_ptr<ICondition> data)
		{
			ImGui::PushID(data.get());
			if (ImGui::Selectable(ScriptSystem::conditionList[data->GetType()].c_str(), data == selectedCondition))
			{
				if (selectedCondition == data)
				{
					selectedCondition = nullptr;
				}
				else
				{
					selectedCondition = data;
				}
				selectedTrigger = nullptr;
				selectedAction = nullptr;
			}

			if ((selectedCondition == data) && ImGui::IsItemClicked(ImGuiMouseButton_Right))
			{
				ImGui::OpenPopup(ScriptSystem::conditionList[data->GetType()].c_str());
			}

			PopUpDataEdit<ICondition>(data.get());

			ImGui::PopID();
		}

		void Module_ScriptEditor::DrawAction(std::shared_ptr<IAction> data)
		{
			ImGui::PushID(data.get());
			if (ImGui::Selectable(ScriptSystem::actionList[data->GetType()].c_str(), data == selectedAction))
			{
				if (selectedAction == data)
				{
					selectedAction = nullptr;
				}
				else
				{
					selectedAction = data;
				}
				selectedTrigger = nullptr;
				selectedCondition = nullptr;
			}

			if ((selectedAction == data) && ImGui::IsItemClicked(ImGuiMouseButton_Right))
			{
				ImGui::OpenPopup(ScriptSystem::actionList[data->GetType()].c_str());
			}

			PopUpDataEdit<IAction>(data.get());

			ImGui::PopID();
		}
	}
}
