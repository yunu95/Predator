#include "Module_SoundEditor.h"

#include "InWanderLand.h"
#include "Application.h"
#include "imgui.h"
#include "imgui_ModuleData.h"
#include "SFXManager.h"

#include "imgui.h"
#include "imgui_Utility.h"
#include <algorithm>

namespace application
{
	namespace editor
	{
		Module_SoundEditor::~Module_SoundEditor()
		{

		}

		void Module_SoundEditor::Initialize()
		{
			SFXManager::SingleInstance();
		}

		void Module_SoundEditor::Update(float ts)
		{

		}

		void Module_SoundEditor::GUIProgress()
		{
			CheckActivation();

			ImGui::Begin("Sound Editor", &activation);

			/// ImGui 관련 내부 변수 업데이트
			isMouseOver = ImGui::IsWindowHovered();
			isFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);

			/// 실제 패널에 그리는 영역
			DrawLayout();

			ImGui::End();
		}
		void Module_SoundEditor::Finalize()
		{

		}

		Module_SoundEditor::Module_SoundEditor()
		{

		}

		void Module_SoundEditor::DrawLayout()
		{
			if (ImGui::BeginTabBar("SoundEditorTab", ImGuiTabBarFlags_None))
			{
				if (ImGui::BeginTabItem("Sounds"))
				{
					DrawSoundTab();
					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("SoundGroups"))
				{
					DrawSoundGroupTab();
					ImGui::EndTabItem();
				}

				ImGui::EndTabBar();
			}
		}

		void Module_SoundEditor::DrawSoundTab()
		{
			static std::string selectedSound = "None";

			static int idx = 0;
			imgui::BeginSection_2Col(idx, "Sounds", ImGui::GetContentRegionAvail().x, 0.2f);

			static std::vector<std::string> selections = std::vector<std::string>();
			if (selections.empty())
			{
				for (auto& each : SFXManager::GetLoadedSFXList())
				{
					selections.push_back(each);
				}
				std::sort(selections.begin(), selections.end());
			}

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			imgui::SmartStyleColor textColor(ImGuiCol_Text, IM_COL32(180, 180, 180, 255));
			ImGui::AlignTextToFramePadding();
			ImGui::Text("Selected Sound");
			ImGui::TableSetColumnIndex(1);
			ImGui::PushItemWidth(-1);

			imgui::SmartStyleColor textColor2(ImGuiCol_Text, IM_COL32_WHITE);

			if (ImGui::BeginCombo("##Selected Sound Path", selectedSound.c_str()))
			{
				for (int i = 0; i < selections.size(); i++)
				{
					const bool is_selected = (selectedSound == selections[i]);
					if (ImGui::Selectable(selections[i].c_str(), is_selected))
					{
						selectedSound = selections[i];
					}

					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}

			ImGui::PopItemWidth();

			imgui::ShiftCursorY(10);

			if (selectedSound != "None")
			{
				int priority = SFXManager::GetSFXPriority(selectedSound);
				ImGui::PushID(("Priority" + selectedSound).c_str());
				if (imgui::DragInt_2Col("Priority", priority, true, 1.0f, 0, 255))
				{
					SFXManager::SetSFXPriority(selectedSound, priority);
				}
				ImGui::PopID();

				static std::vector<std::string> groupNameList = std::vector<std::string>();
				static std::vector<const char*> groupPtrList = std::vector<const char*>();
				groupNameList.clear();
				groupPtrList.clear();
				int nameIdx = 0;
				for (auto& each : SFXManager::GetSoundGroupNames())
				{
					groupNameList.push_back("[" + to_string(nameIdx) + "] " + each);
					groupPtrList.push_back(groupNameList[nameIdx].c_str());
					nameIdx++;
				}

				int selected = SFXManager::GetSFXGroupIndex(selectedSound);
				if (imgui::Dropdown_2Col("Sound Group", &groupPtrList[0], groupPtrList.size(), &selected))
				{
					SFXManager::SettingSFXGroup(selectedSound, selected);
				}

				bool randomFlag = SFXManager::GetSFXRandomFlag(selectedSound);
				ImGui::PushID(("RandomFlag" + selectedSound).c_str());
				if (imgui::Checkbox_2Col("RandomFlag", randomFlag))
				{
					SFXManager::SetSFXRandomFlag(selectedSound, randomFlag);
				}
				ImGui::PopID();

				int containerSize = SFXManager::GetSFXRandomSize(selectedSound);
				ImGui::PushID(("RandomListSize" + selectedSound).c_str());
				if (imgui::DragInt_2Col("RandomListSize", containerSize, true, 1.0f, 0, 255))
				{
					SFXManager::SetSFXRandomSize(selectedSound, containerSize);
				}
				ImGui::PopID();

				int resourceIdx = 0;
				for (auto& each : SFXManager::GetSFXRandomResorces(selectedSound))
				{
					static std::vector<const char*> resourceList = std::vector<const char*>();
					static std::vector<std::string> sortedList = std::vector<std::string>();
					if (resourceList.empty())
					{
						for (auto& each : SFXManager::GetLoadedSFXList())
						{
							sortedList.push_back(each);
						}

						std::sort(sortedList.begin(), sortedList.end());

						for (auto& each : sortedList)
						{
							resourceList.push_back(each.c_str());
						}
					}

					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					imgui::SmartStyleColor textColor(ImGuiCol_Text, IM_COL32(180, 180, 180, 255));
					ImGui::AlignTextToFramePadding();
					ImGui::Text(("List [" + std::to_string(resourceIdx) + "]").c_str());
					ImGui::TableSetColumnIndex(1);
					ImGui::PushItemWidth(-1);

					std::string current = SFXManager::GetSFXRandomResorces(selectedSound)[resourceIdx];
					if (ImGui::BeginCombo(("##RandomSoundList" + to_string(resourceIdx)).c_str(), current.c_str()))
					{
						for (int j = 0; j < resourceList.size(); j++)
						{
							const bool is_selected = (current == resourceList[j]);
							if (ImGui::Selectable(resourceList[j], is_selected))
							{
								SFXManager::ChangeSFXRandomResorce(selectedSound, resourceIdx, resourceList[j]);
							}

							if (is_selected)
								ImGui::SetItemDefaultFocus();
						}
						ImGui::EndCombo();
					}

					resourceIdx++;
				}
			}
			imgui::EndSection();
		}

		void Module_SoundEditor::DrawSoundGroupTab()
		{
			auto width = ImGui::GetContentRegionAvail().x - 4;
			static std::vector<float> tableRatio = std::vector<float>();
			if (tableRatio.empty())
			{
				tableRatio.resize(11);
				tableRatio[0] = 0.04f;
				tableRatio[1] = 0.10f;
				tableRatio[2] = 0.04f;
				tableRatio[3] = 0.24f;
				tableRatio[4] = 0.07f;
				tableRatio[5] = 0.07f;
				tableRatio[6] = 0.10f;
				tableRatio[7] = 0.04f;
				tableRatio[8] = 0.07f;
				tableRatio[9] = 0.07f;
				tableRatio[10] = 0.07f;
			}

			static std::vector<const char*> tableID = std::vector<const char*>();
			if (tableID.empty())
			{
				tableID.resize(11);
				tableID[0] = "ID";
				tableID[1] = "NAME";
				tableID[2] = "USE";
				tableID[3] = "VOLUME";
				tableID[4] = "MAX";
				tableID[5] = "OPTION";
				tableID[6] = "Priority";
				tableID[7] = "IsFading";
				tableID[8] = "FadeRatio";
				tableID[9] = "FadeOutDuration";
				tableID[10] = "FadeInDuration";
			}

			if (ImGui::BeginTable("##Sound_Group_Editor", tableRatio.size(), ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_Borders))
			{
				for (int i = 0; i < tableRatio.size(); i++)
				{
					ImGui::TableSetupColumn(tableID[i], ImGuiTableColumnFlags_WidthFixed, width * tableRatio[i]);
				}
				ImGui::TableHeadersRow();
			}

			auto cellPadding = ImGui::GetStyle().CellPadding.x;

			auto& names = SFXManager::GetSoundGroupNames();
			static std::vector<bool> beforeFocus = std::vector<bool>(names.size(), false);
			for (int i = 0; i < names.size(); i++)
			{
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::SetNextItemWidth(ImGui::GetColumnWidth() - cellPadding);
				ImGui::Text(to_string(i).c_str());
				ImGui::TableSetColumnIndex(1);
				if (i != 0)
				{
					ImGui::SetNextItemWidth(ImGui::GetColumnWidth() - cellPadding);
					if (ImGui::InputText(("##GroupName" + to_string(i)).c_str(), &names[i][0], 32))
					{
						wanderUtils::UpdateStringSize(names[i]);
					}

					if (ImGui::IsItemFocused())
					{
						beforeFocus[i] = true;
						EditorLayer::SetInputControl(false);
						EditorCamera::GetSingletonInstance().SetInputUpdate(false);
					}
					else if (beforeFocus[i])
					{
						beforeFocus[i] = false;
						EditorLayer::SetInputControl(true);
						EditorCamera::GetSingletonInstance().SetInputUpdate(true);
					}
				}
				else
				{
					ImGui::SetNextItemWidth(ImGui::GetColumnWidth() - cellPadding);
					ImGui::Text(names[i].c_str());
				}

				ImGui::TableSetColumnIndex(2);
				int count = SoundSystem::GetSoundGroupUseCount(i) / 2;
				ImGui::SetNextItemWidth(ImGui::GetColumnWidth() - cellPadding);
				ImGui::Text(to_string(count).c_str());

				ImGui::TableSetColumnIndex(3);
				float volume = SFXManager::GetSFXGroupVolume(i);
				ImGui::SetNextItemWidth(ImGui::GetColumnWidth() - cellPadding);
				if (ImGui::SliderFloat(("##Volume" + to_string(i)).c_str(), &volume, 0, 1))
				{
					SFXManager::SetSFXGroupVolume(i, volume);
				}

				ImGui::TableSetColumnIndex(4);
				int maxCount = SFXManager::GetSFXGroupMaxAudible(i);
				ImGui::SetNextItemWidth(ImGui::GetColumnWidth() - cellPadding);
				if (ImGui::DragInt(("##MaxAudible" + to_string(i)).c_str(), &maxCount, 1.0f, -1, 1000))
				{
					SFXManager::SetSFXGroupMaxAudible(i, maxCount);
				}

				ImGui::TableSetColumnIndex(5);
				auto option = SFXManager::GetSFXGroupMaxAudibleBehavior(i);
				static std::vector<const char*> optionList = std::vector<const char*>();
				if (optionList.empty())
				{
					optionList.push_back("Fail");
					optionList.push_back("Mute");
					optionList.push_back("Steal");
				}

				std::string current = optionList[(int)option];
				ImGui::SetNextItemWidth(ImGui::GetColumnWidth() - cellPadding);
				if (ImGui::BeginCombo(("##SoundGroupOptionCombo" + to_string(i)).c_str(), current.c_str()))
				{
					for (int j = 0; j < optionList.size(); j++)
					{
						const bool is_selected = (current == optionList[j]);
						if (ImGui::Selectable(optionList[j], is_selected))
						{
							SFXManager::SetSFXGroupMaxAudibleBehavior(i, (SoundSystem::SOUNDGROUP_BEHAVIOR)j);
						}

						if (is_selected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}

				ImGui::TableSetColumnIndex(6);
				int priority = SFXManager::GetSFXGroupPriority(i);
				ImGui::SetNextItemWidth(ImGui::GetColumnWidth() - cellPadding);
				if (ImGui::DragInt(("##SoundGroupPriority" + to_string(i)).c_str(), &priority, 1.0f, 0, 255))
				{
					SFXManager::SetSFXGroupPriority(i, priority);
				}

				ImGui::TableSetColumnIndex(7);
				bool flag = SFXManager::GetSFXGroupPriorityFlag(i);
				ImGui::SetNextItemWidth(ImGui::GetColumnWidth() - cellPadding);
				if (ImGui::Checkbox(("##SoundGroupFlag" + to_string(i)).c_str(), &flag))
				{
					SFXManager::SetSFXGroupPriorityFlag(i, flag);
				}

				ImGui::TableSetColumnIndex(8);
				float fadeRatio = SFXManager::GetSFXGroupPriorityFadeRatio(i);
				ImGui::SetNextItemWidth(ImGui::GetColumnWidth() - cellPadding);
				if (ImGui::DragFloat(("##SoundGroupFadeRatio" + to_string(i)).c_str(), &fadeRatio, 0.1f, 0, 1))
				{
					SFXManager::SetSFXGroupPriorityFadeRatio(i, fadeRatio);
				}

				ImGui::TableSetColumnIndex(9);
				float fadeOutDuration = SFXManager::GetSFXGroupPriorityFadeOutTime(i);
				ImGui::SetNextItemWidth(ImGui::GetColumnWidth() - cellPadding);
				if (ImGui::DragFloat(("##SoundGroupFadeOutT" + to_string(i)).c_str(), &fadeOutDuration, 0.1f, 0, 100))
				{
					SFXManager::SetSFXGroupPriorityFadeOutTime(i, fadeOutDuration);
				}

				ImGui::TableSetColumnIndex(10);
				float fadeInDuration = SFXManager::GetSFXGroupPriorityFadeInTime(i);
				ImGui::SetNextItemWidth(ImGui::GetColumnWidth() - cellPadding);
				if (ImGui::DragFloat(("##SoundGroupFadeInT" + to_string(i)).c_str(), &fadeInDuration, 0.1f, 0, 100))
				{
					SFXManager::SetSFXGroupPriorityFadeInTime(i, fadeInDuration);
				}
			}

			ImGui::EndTable();
		}
	}
}
