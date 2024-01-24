#include "imgui_Utility.h"

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
