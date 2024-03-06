/// 2024. 02. 22 김상준
/// TemplateData Editor 에서 사용할 TemplateData 의 데이터 필드를 시각화 하기 위한
/// template 함수들을 정리한 헤더 파일

#pragma once

#include "imgui_Utility.h"

#include "PodStructs.h"
#include "EditorResourceManager.h"
#include "TemplateDataManager.h"

namespace application
{
	namespace editor
	{
		namespace imgui
		{
			namespace data
			{
				// imgui 를 통해 데이터 편집이 가능한 내용을 보여주는 부분으로,
				// 사용자 정의 자료형에 대해서는 이 템플릿 함수에 대한 특수화를 구현하여
				// 지원하도록 함
				template <typename T>
				bool DrawData(std::string label, T& data)
				{
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					imgui::SmartStyleColor textColor(ImGuiCol_Text, IM_COL32(180, 180, 180, 255));
					ImGui::AlignTextToFramePadding();
					ImGui::Text(label.c_str());
					ImGui::TableSetColumnIndex(1);
					return false;
				}

				template <>
				bool DrawData(std::string label, const int& data)
				{
					return DragInt_2Col(label, const_cast<int&>(data));
				}

				template <>
				bool DrawData(std::string label, const float& data)
				{
					return DragFloat_2Col(label, const_cast<float&>(data));
				}

				template <>
				bool DrawData(std::string label, const double& data)
				{
					float val = data;
					bool returnVal = DragFloat_2Col(label, val);
					const_cast<double&>(data) = val;
					return returnVal;
				}

				template <>
				bool DrawData(std::string label, const bool& data)
				{
					return Checkbox_2Col(label, const_cast<bool&>(data));
				}

				template <>
				bool DrawData(std::string label, const std::string& data)
				{
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					imgui::SmartStyleColor textColor(ImGuiCol_Text, IM_COL32(180, 180, 180, 255));
					ImGui::AlignTextToFramePadding();
					ImGui::Text(label.c_str());
					ImGui::TableSetColumnIndex(1);
					ImGui::PushItemWidth(-1);

					imgui::SmartStyleColor textColor2(ImGuiCol_Text, IM_COL32_WHITE);

					if (label == "fbxName")
					{
						bool returnVal = false;
						static auto& fbxSet = ResourceManager::GetSingletonInstance().GetFbxList();
						static std::vector<std::string> selections = std::vector<std::string>();
						std::string current = data;

						if (selections.empty())
						{
							for (auto each : fbxSet)
							{
								selections.push_back(each);
							}
						}

						if (ImGui::BeginCombo("##fbxCombo", data.c_str()))
						{
							for (int i = 0; i < fbxSet.size(); i++)
							{
								const bool is_selected = (current == selections[i]);
								if (ImGui::Selectable(selections[i].c_str(), is_selected))
								{
									current = selections[i];
									const_cast<std::string&>(data) = current;
									TemplateDataManager::GetSingletonInstance().GetTemplateData(current)->SetDataResourceName(current);
									returnVal = true;
								}

								if (is_selected)
									ImGui::SetItemDefaultFocus();
							}
							ImGui::EndCombo();
						}
						ImGui::PopItemWidth();
						return returnVal;
					}
					else
					{
						ImGui::Text(data.c_str());
						ImGui::PopItemWidth();
						return false;
					}
				}

				template <typename T, int N = 0>
				void DrawDataField(const T& data)
				{
					if constexpr (N == boost::pfr::tuple_size_v<T>)
					{
						return;
					}
					else
					{
						DrawData(std::string(boost::pfr::get_name<N, T>()), boost::pfr::get<N>(data));
						DrawDataField<T, N + 1>(data);
					}
				}
			}
		}
	}
}
