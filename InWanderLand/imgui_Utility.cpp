#include "imgui_Utility.h"

#include "EditorResourceManager.h"

namespace application
{
	namespace editor
	{
		namespace imgui
		{
			ImRect RectExpanded(const ImRect& rect, float x, float y)
			{
				ImRect result = rect;
				result.Min.x -= x;
				result.Min.y -= y;
				result.Max.x += x;
				result.Max.y += y;
				return result;
			}

			bool IsItemHovered(float delayInSeconds, ImGuiHoveredFlags flags)
			{
				return ImGui::IsItemHovered() && GImGui->HoveredIdTimer > delayInSeconds; /*HoveredIdNotActiveTimer*/
			}

			void SetTooltip(std::string text, float delayInSeconds, bool allowWhenDisabled, ImVec2 padding)
			{
				if (IsItemHovered(delayInSeconds, allowWhenDisabled ? ImGuiHoveredFlags_AllowWhenDisabled : 0))
				{
					imgui::SmartStyleVar tooltipPadding(ImGuiStyleVar_WindowPadding, padding);
					imgui::SmartStyleColor textCol(ImGuiCol_Text, IM_COL32(210, 210, 210, 255));
					ImGui::SetTooltip(text.data());
				}
			}

			void ShiftCursorX(float distance)
			{
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + distance);
			}

			void ShiftCursorY(float distance)
			{
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + distance);
			}

			void ShiftCursor(float x, float y)
			{
				const ImVec2 cursor = ImGui::GetCursorPos();
				ImGui::SetCursorPos(ImVec2(cursor.x + x, cursor.y + y));
			}

			bool SelectableImageButton(std::string imageKey, std::string filename, bool selected, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1, bool useTint, const ImVec4& bgColor, const ImVec4& selectedColor)
			{
				static auto& erm = application::editor::ResourceManager::GetSingletonInstance();
				auto button = erm.GetTexture2D(filename);

				if (button == nullptr)
					return false;

				if (useTint)
				{
					if (selected)
					{
						return ImGui::ImageButton(imageKey.c_str(), (ImTextureID)button->GetID(), size, uv0, uv1, bgColor, selectedColor);
					}
					else
					{
						return ImGui::ImageButton(imageKey.c_str(), (ImTextureID)button->GetID(), size, uv0, uv1, bgColor);
					}
				}
				else
				{
					if (selected)
					{
						imgui::SmartStyleColor buttonCol(ImGuiCol_Button, selectedColor);
						return ImGui::ImageButton(imageKey.c_str(), (ImTextureID)button->GetID(), size, uv0, uv1, bgColor);
					}
					else
					{
						return ImGui::ImageButton(imageKey.c_str(), (ImTextureID)button->GetID(), size, uv0, uv1, bgColor);
					}
				}
			}

			bool BeginSection_1Col(int& countIdx, std::string sectionName, float width, ImColor underlineColor)
			{
				if (countIdx > 0)
					imgui::ShiftCursorY(5.5f);

				imgui::SmartStyleColor textColor(ImGuiCol_Text, IM_COL32_WHITE);
				ImGui::Text(sectionName.c_str());
				imgui::draw::Underline(underlineColor);
				imgui::ShiftCursorY(3.5f);

				bool result = ImGui::BeginTable("##section_table", 1, ImGuiTableFlags_SizingStretchSame);
				if (result)
				{
					ImGui::TableSetupColumn("Labels && Widgets", ImGuiTableColumnFlags_WidthFixed, width);
				}

				countIdx++;
				return result;
			}

			bool BeginSection_2Col(int& countIdx, std::string sectionName, float width, float firstColRatio, ImColor underlineColor)
			{
				if (countIdx > 0)
					imgui::ShiftCursorY(5.5f);

				imgui::SmartStyleColor textColor(ImGuiCol_Text, IM_COL32_WHITE);
				ImGui::Text(sectionName.c_str());
				imgui::draw::Underline(underlineColor);
				imgui::ShiftCursorY(3.5f);

				bool result = ImGui::BeginTable("##section_table", 2, ImGuiTableFlags_SizingStretchSame);
				if (result)
				{
					ImGui::TableSetupColumn("Labels", ImGuiTableColumnFlags_WidthFixed, width * firstColRatio);
					ImGui::TableSetupColumn("Widgets", ImGuiTableColumnFlags_WidthFixed, width * (1.0f - firstColRatio));
				}

				countIdx++;
				return result;
			}

			bool SliderFloat_2Col(std::string valName, float& value, float min, float max)
			{
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				imgui::SmartStyleColor textColor(ImGuiCol_Text, IM_COL32(180, 180, 180, 255));
				ImGui::AlignTextToFramePadding();
				ImGui::Text(valName.c_str());
				ImGui::TableSetColumnIndex(1);
				ImGui::SetNextItemWidth(-1);
				return ImGui::SliderFloat(("##" + valName).c_str(), &value, min, max);
			}

			bool SliderInt_2Col(std::string valName, int& value, int min, int max)
			{
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				imgui::SmartStyleColor textColor(ImGuiCol_Text, IM_COL32(180, 180, 180, 255));
				ImGui::AlignTextToFramePadding();
				ImGui::Text(valName.c_str());
				ImGui::TableSetColumnIndex(1);
				ImGui::SetNextItemWidth(-1);
				return ImGui::SliderInt(("##" + valName).c_str(), &value, min, max);
			}

			bool DragFloat_2Col(std::string valName, float& value, float speed, float min, float max)
			{
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				imgui::SmartStyleColor textColor(ImGuiCol_Text, IM_COL32(180, 180, 180, 255));
				ImGui::AlignTextToFramePadding();
				ImGui::Text(valName.c_str());
				ImGui::TableSetColumnIndex(1);
				ImGui::SetNextItemWidth(-1);
				return ImGui::DragFloat(("##" + valName).c_str(), &value, speed, min, max);
			}

			bool DragInt_2Col(std::string valName, int& value, float speed, int min, int max)
			{
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				imgui::SmartStyleColor textColor(ImGuiCol_Text, IM_COL32(180, 180, 180, 255));
				ImGui::AlignTextToFramePadding();
				ImGui::Text(valName.c_str());
				ImGui::TableSetColumnIndex(1);
				ImGui::SetNextItemWidth(-1);
				return ImGui::DragInt(("##" + valName).c_str(), &value, speed, min, max);
			}

			bool Checkbox_2Col(std::string valName, bool& value)
			{
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				imgui::SmartStyleColor textColor(ImGuiCol_Text, IM_COL32(180, 180, 180, 255));
				ImGui::AlignTextToFramePadding();
				ImGui::Text(valName.c_str());
				ImGui::TableSetColumnIndex(1);
				auto table = ImGui::GetCurrentTable();
				float columnWidth = ImGui::TableGetMaxColumnWidth(table, 1);
				imgui::ShiftCursorX(columnWidth - ImGui::GetFrameHeight() - ImGui::GetStyle().ItemInnerSpacing.x);
				return ImGui::Checkbox(("##" + valName).c_str(), &value);
			}

			bool Dropdown_2Col(std::string valName, const char** options, int optionCount, int* selected)
			{
				const char* current = options[*selected];
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				imgui::SmartStyleColor textColor(ImGuiCol_Text, IM_COL32(180, 180, 180, 255));
				ImGui::AlignTextToFramePadding();
				ImGui::Text(valName.c_str());
				ImGui::TableSetColumnIndex(1);
				ImGui::PushItemWidth(-1);

				imgui::SmartStyleColor textColor2(ImGuiCol_Text, IM_COL32_WHITE);
				bool result = false;
				if (ImGui::BeginCombo("##Combo", current))
				{
					for (int i = 0; i < optionCount; i++)
					{
						const bool is_selected = (current == options[i]);
						if (ImGui::Selectable(options[i], is_selected))
						{
							current = options[i];
							*selected = i;
							result = true;
						}

						if (is_selected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}

				ImGui::PopItemWidth();

				return result;
			}

			void EndSection()
			{
				ImGui::EndTable();
			}
			
			namespace draw
			{
				void Underline(ImColor color, bool fullWidth, float offsetX, float offsetY)
				{
					if (fullWidth)
					{
						if (ImGui::GetCurrentWindow()->DC.CurrentColumns != nullptr)
							ImGui::PushColumnsBackground();
						else if (ImGui::GetCurrentTable() != nullptr)
							ImGui::TablePushBackgroundChannel();
					}

					const float width = fullWidth ? ImGui::GetWindowWidth() : ImGui::GetContentRegionAvail().x;
					const ImVec2 cursor = ImGui::GetCursorScreenPos();
					ImGui::GetWindowDrawList()->AddLine(ImVec2(cursor.x + offsetX, cursor.y + offsetY),
						ImVec2(cursor.x + width, cursor.y + offsetY),
						color, 1.0f);

					if (fullWidth)
					{
						if (ImGui::GetCurrentWindow()->DC.CurrentColumns != nullptr)
							ImGui::PopColumnsBackground();
						else if (ImGui::GetCurrentTable() != nullptr)
							ImGui::TablePopBackgroundChannel();
					}
				}
			}
		}
	}
}
