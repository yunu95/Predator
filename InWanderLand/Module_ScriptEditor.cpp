#include "InWanderLand.h"
#include "Module_ScriptEditor.h"

#include "WanderUtils.h"
#include "Application.h"
#include "EditorLayer.h"
#include "EditorInputManager.h"
#include "EditorPopupManager.h"
#include "Script.h"
#include "Panel_Palette.h"

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
			static auto& epm = EditorPopupManager::GetSingletonInstance();
			if (!epm.GetReturnPopupName().empty())
			{
				isEditingPopup = true;
			}
			else
			{
				isEditingPopup = false;
			}

			if (isFocused && !isEditingPopup && eim.IsKeyboardPressed(KeyCode::Delete))
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

			ImGuiWindowFlags flag = 0;
			if (isEditingPopup)
			{
				flag |= ImGuiWindowFlags_NoInputs;
				ImGui::SetNextWindowBgAlpha(0.3f);
				ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.2f, 0.2f, 0.2f, 0.7f));
			}
			ImGui::Begin("Script Editor", &activation, flag);

			/// ImGui 관련 내부 변수 업데이트
			isMouseOver = ImGui::IsWindowHovered();
			isFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);

			/// 실제 패널에 그리는 영역
			DrawLayout();

			if (isEditingPopup)
			{
				ImGui::PopStyleColor();
			}

			/// Popup 으로 인해 특별한 처리가 필요한 경우
			if (isEditingPopup)
			{
				static auto& epm = EditorPopupManager::GetSingletonInstance();
				static auto& pp = PalettePanel::GetSingletonInstance();

				static auto& pm = palette::PaletteManager::GetSingletonInstance();
				static auto& tp = palette::TerrainPalette::SingleInstance();
				static auto& up = palette::UnitPalette::SingleInstance();
				static auto& op = palette::OrnamentPalette::SingleInstance();
				static auto& rp = palette::RegionPalette::SingleInstance();
				static auto& wp = palette::WavePalette::SingleInstance();
				static auto& cp = palette::CameraPalette::SingleInstance();
				static auto& lp = palette::LightPalette::SingleInstance();

				bool pop = true;
				ImGuiWindowFlags flag = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar;
				ImVec2 center = ImGui::GetWindowViewport()->GetCenter();
				ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
				ImGui::SetNextWindowSize(ImVec2(300, 100));

				/// Trigger
				{
					/// EnterRegion
					if (epm.GetReturnPopupName() == "SetEnterRegion")
					{
						ImGui::Begin("Enter Region Popup", &pop, flag);
						auto rect = ImGui::GetContentRegionAvail();
						auto size = ImGui::CalcTextSize("Please Setting Enter Region");
						imgui::ShiftCursorX((rect.x - size.x) / 2);
						imgui::ShiftCursorY((rect.y - size.y) / 2);
						ImGui::Text("Please Setting Enter Region");
						ImGui::End();

						pp.ChangeTab("Region");

						auto data = epm.GetReturnPopupData<Trigger_EnterRegion>();
						if (data->isEditing == false && pm.GetCurrentPalette() == &rp)
						{
							data->isEditing = true;
							rp.Reset();
						}
							
						if(data->isEditing == true && rp.GetSelections().size() == 1)
						{
							data->SetRegion(static_cast<RegionData*>(*rp.GetSelections().begin()));
							data->isEditing = false;
							epm.Return();
						}

						if (!pop)
						{
							data->isEditing = false;
							epm.Return();
						}
					}

					/// LeaveRegion
					else if (epm.GetReturnPopupName() == "SetLeaveRegion")
					{
						ImGui::Begin("Leave Region Popup", &pop, flag);
						auto rect = ImGui::GetContentRegionAvail();
						auto size = ImGui::CalcTextSize("Please Setting Leave Region");
						imgui::ShiftCursorX((rect.x - size.x) / 2);
						imgui::ShiftCursorY((rect.y - size.y) / 2);
						ImGui::Text("Please Setting Leave Region");
						ImGui::End();

						pp.ChangeTab("Region");

						auto data = epm.GetReturnPopupData<Trigger_LeaveRegion>();
						if (data->isEditing == false && pm.GetCurrentPalette() == &rp)
						{
							data->isEditing = true;
							rp.Reset();
						}

						if (data->isEditing == true && rp.GetSelections().size() == 1)
						{
							data->SetRegion(static_cast<RegionData*>(*rp.GetSelections().begin()));
							data->isEditing = false;
							epm.Return();
						}

						if (!pop)
						{
							data->isEditing = false;
							epm.Return();
						}
					}

					/// UnitAppear
					else if (epm.GetReturnPopupName() == "SetUnitAppear")
					{
						ImGui::Begin("UnitAppear Popup", &pop, flag);
						auto rect = ImGui::GetContentRegionAvail();
						auto size = ImGui::CalcTextSize("Please Setting Target Unit");
						imgui::ShiftCursorX((rect.x - size.x) / 2);
						imgui::ShiftCursorY((rect.y - size.y) / 2);
						ImGui::Text("Please Setting Target Unit");
						ImGui::End();

						pp.ChangeTab("Unit");

						auto data = epm.GetReturnPopupData<Trigger_UnitAppear>();
						if (data->isEditing == false && pm.GetCurrentPalette() == &up)
						{
							data->isEditing = true;
							up.Reset();
						}

						if (data->isEditing == true && up.GetSelections().size() == 1)
						{
							data->SetUnit(static_cast<UnitData*>(*up.GetSelections().begin()));
							data->isEditing = false;
							epm.Return();
						}

						if (!pop)
						{
							data->isEditing = false;
							epm.Return();
						}
					}

					/// UnitDie
					else if (epm.GetReturnPopupName() == "SetUnitDie")
					{
						ImGui::Begin("UnitDie Popup", &pop, flag);
						auto rect = ImGui::GetContentRegionAvail();
						auto size = ImGui::CalcTextSize("Please Setting Target Unit");
						imgui::ShiftCursorX((rect.x - size.x) / 2);
						imgui::ShiftCursorY((rect.y - size.y) / 2);
						ImGui::Text("Please Setting Target Unit");
						ImGui::End();

						pp.ChangeTab("Unit");

						auto data = epm.GetReturnPopupData<Trigger_UnitDie>();
						if (data->isEditing == false && pm.GetCurrentPalette() == &up)
						{
							data->isEditing = true;
							up.Reset();
						}

						if (data->isEditing == true && up.GetSelections().size() == 1)
						{
							data->SetUnit(static_cast<UnitData*>(*up.GetSelections().begin()));
							data->isEditing = false;
							epm.Return();
						}

						if (!pop)
						{
							data->isEditing = false;
							epm.Return();
						}
					}
				}

				/// Condition
				{

				}

				/// Action
				{
					if (epm.GetReturnPopupName() == "SetTargetCamera")
					{
						ImGui::Begin("Camera Change Popup", &pop, flag);
						auto rect = ImGui::GetContentRegionAvail();
						auto size = ImGui::CalcTextSize("Please Setting Camera");
						imgui::ShiftCursorX((rect.x - size.x) / 2);
						imgui::ShiftCursorY((rect.y - size.y) / 2);
						ImGui::Text("Please Setting Camera");
						ImGui::End();

						pp.ChangeTab("Cam");

						auto data = epm.GetReturnPopupData<Action_CameraChangeView>();
						if (data->isEditing == false && pm.GetCurrentPalette() == &cp)
						{
							data->isEditing = true;
							cp.Reset();
						}

						if (data->isEditing == true && cp.GetSelections().size() == 1)
						{
							data->SetCamera(static_cast<CameraData*>(*cp.GetSelections().begin()));
							data->isEditing = false;
							epm.Return();
						}

						if (!pop)
						{
							data->isEditing = false;
							epm.Return();
						}
					}
				}
			}

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
			isEditingPopup = false;
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
			ImGuiWindowFlags flag = 0;
			if (isEditingPopup)
			{
				flag |= ImGuiWindowFlags_NoInputs;
			}
			ImGui::BeginChild("ScriptList", region, true, flag);
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
			ImGuiWindowFlags flag = 0;
			if (isEditingPopup)
			{
				flag |= ImGuiWindowFlags_NoInputs;
			}
			ImGui::BeginChild("SelectedScript", region, true, flag);

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
				int finalIdx = -1;
				int offset = 0;
				int idx = 0;
				for (auto& action : data->actionList)
				{
					DrawAction(action);
					offset = ImGui::ItemReorder("ActionReorder", true);
					if (offset != 0 && idx + offset >= 0)
					{
						finalIdx = idx + offset;
					}
					idx++;
				}

				if (finalIdx >= 0)
				{
					selectedScript->ReorderAction(selectedAction, finalIdx);
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
								case application::TriggerType::RepeatPeriodically:
								{
									selectedScript->AddTrigger<Trigger_RepeatPeriodically>();
									break;
								}
								case application::TriggerType::RepeatPeriodicallyRealTime:
								{
									selectedScript->AddTrigger<Trigger_RepeatPeriodicallyRealTime>();
									break;
								}
								case application::TriggerType::UnitAppear:
								{
									selectedScript->AddTrigger<Trigger_UnitAppear>();
									break;
								}
								case application::TriggerType::UnitDie:
								{
									selectedScript->AddTrigger<Trigger_UnitDie>();
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
								case application::ConditionType::CinematicModeOn:
								{
									selectedScript->AddCondition<Condition_CinematicModeOn>();
									break;
								}
								case application::ConditionType::CinematicModeOff:
								{
									selectedScript->AddCondition<Condition_CinematicModeOff>();
									break;
								}
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
								case application::ActionType::CameraChangeView:
								{
									selectedScript->AddAction<Action_CameraChangeView>();
									break;
								}
								case application::ActionType::CameraSaveView:
								{
									selectedScript->AddAction<Action_CameraSaveView>();
									break;
								}
								case application::ActionType::CameraLoadView:
								{
									selectedScript->AddAction<Action_CameraLoadView>();
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
			if (!data->IsValid())
			{
				auto pos = ImGui::GetCurrentWindow()->DC.CursorPos;
				ImGui::RenderFrame(pos, ImVec2(pos.x + ImGui::GetContentRegionAvail().x, pos.y + ImGui::CalcTextSize("Font").y), IM_COL32(255, 20, 20, 255), false, 0.0f);
				ImGui::PushStyleColor(ImGuiCol_Header, IM_COL32(255, 20, 20, 255));
				ImGui::PushStyleColor(ImGuiCol_HeaderHovered, IM_COL32(200, 20, 20, 255));
				ImGui::PushStyleColor(ImGuiCol_HeaderActive, IM_COL32(255, 20, 20, 255));
			}

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

			if (!data->IsValid())
			{
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();
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
			if (!data->IsValid())
			{
				auto pos = ImGui::GetCurrentWindow()->DC.CursorPos;
				ImGui::RenderFrame(pos, ImVec2(pos.x + ImGui::GetContentRegionAvail().x, pos.y + ImGui::CalcTextSize("Font").y), IM_COL32(255, 20, 20, 255), false, 0.0f);
				ImGui::PushStyleColor(ImGuiCol_Header, IM_COL32(255, 20, 20, 255));
				ImGui::PushStyleColor(ImGuiCol_HeaderHovered, IM_COL32(200, 20, 20, 255));
				ImGui::PushStyleColor(ImGuiCol_HeaderActive, IM_COL32(255, 20, 20, 255));
			}

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

			if (!data->IsValid())
			{
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();
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
			if (!data->IsValid())
			{
				auto pos = ImGui::GetCurrentWindow()->DC.CursorPos;
				ImGui::RenderFrame(pos, ImVec2(pos.x + ImGui::GetContentRegionAvail().x, pos.y + ImGui::CalcTextSize("Font").y), IM_COL32(255, 20, 20, 255), false, 0.0f);
				ImGui::PushStyleColor(ImGuiCol_Header, IM_COL32(255, 20, 20, 255));
				ImGui::PushStyleColor(ImGuiCol_HeaderHovered, IM_COL32(200, 20, 20, 255));
				ImGui::PushStyleColor(ImGuiCol_HeaderActive, IM_COL32(255, 20, 20, 255));
			}

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

			if (!data->IsValid())
			{
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();
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
