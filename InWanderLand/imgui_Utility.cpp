#include "imgui_Utility.h"

#include "EditorResourceManager.h"

extern const float DRAG_MOUSE_THRESHOLD_FACTOR;

#define BIT(x) (1u << x)

#define MESSAGE_BOX_OK_BUTTON BIT(0)
#define MESSAGE_BOX_CANCEL_BUTTON BIT(1)
#define MESSAGE_BOX_USER_FUNC BIT(2)

#include <iostream>

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

			bool SliderFloat_2Col(std::string valName, float& value, bool editable, float min, float max)
			{
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				imgui::SmartStyleColor textColor(ImGuiCol_Text, IM_COL32(180, 180, 180, 255));
				ImGui::AlignTextToFramePadding();
				ImGui::Text(valName.c_str());
				ImGui::TableSetColumnIndex(1);
				ImGui::SetNextItemWidth(-1);
				ImGuiSliderFlags flag = 0;
				if (!editable)
					flag = ImGuiSliderFlags_NoInput;
				return ImGui::SliderFloat(("##" + valName).c_str(), &value, min, max, "%.3f", flag);
			}

			bool SliderInt_2Col(std::string valName, int& value, bool editable, int min, int max)
			{
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				imgui::SmartStyleColor textColor(ImGuiCol_Text, IM_COL32(180, 180, 180, 255));
				ImGui::AlignTextToFramePadding();
				ImGui::Text(valName.c_str());
				ImGui::TableSetColumnIndex(1);
				ImGui::SetNextItemWidth(-1);
				ImGuiSliderFlags flag = 0;
				if (!editable)
					flag = ImGuiSliderFlags_NoInput;
				return ImGui::SliderInt(("##" + valName).c_str(), &value, min, max, "%d", flag);
			}

			bool DragFloat_2Col(std::string valName, float& value, bool editable, float speed, float min, float max)
			{
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				imgui::SmartStyleColor textColor(ImGuiCol_Text, IM_COL32(180, 180, 180, 255));
				ImGui::AlignTextToFramePadding();
				ImGui::Text(valName.c_str());
				ImGui::TableSetColumnIndex(1);
				ImGui::SetNextItemWidth(-1);
				ImGuiSliderFlags flag = 0;
				if (!editable)
					flag = ImGuiSliderFlags_NoInput;
				return ImGui::DragFloat(("##" + valName).c_str(), &value, speed, min, max, "%.3f", flag);
			}

			bool DragInt_2Col(std::string valName, int& value, bool editable, float speed, int min, int max)
			{
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				imgui::SmartStyleColor textColor(ImGuiCol_Text, IM_COL32(180, 180, 180, 255));
				ImGui::AlignTextToFramePadding();
				ImGui::Text(valName.c_str());
				ImGui::TableSetColumnIndex(1);
				ImGui::SetNextItemWidth(-1);
				ImGuiSliderFlags flag = 0;
				if (!editable)
					flag = ImGuiSliderFlags_NoInput;
				return ImGui::DragInt(("##" + valName).c_str(), &value, speed, min, max, "%d", flag);
			}

			bool Checkbox_2Col(std::string valName, bool& value)
			{
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				imgui::SmartStyleColor textColor(ImGuiCol_Text, IM_COL32(180, 180, 180, 255));
				ImGui::AlignTextToFramePadding();
				ImGui::Text(valName.c_str());
				ImGui::TableSetColumnIndex(1);
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

			bool Vector2_2Col(std::string valName, float& x, float& y, float reset_x, float reset_y)
			{
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				imgui::SmartStyleColor textColor(ImGuiCol_Text, IM_COL32(180, 180, 180, 255));
				ImGui::AlignTextToFramePadding();
				ImGui::Text(valName.c_str());
				ImGui::TableSetColumnIndex(1);

				imgui::SmartStyleVar itemPadding = imgui::SmartStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(1, 2));

				bool returnVal = false;

				const float rowWidth = ImGui::GetFontSize() + 4;
				const float boxWidth = ImGui::GetColumnWidth() / 2 - rowWidth - 2;
				const ImVec2 buttonSize = ImVec2(rowWidth, rowWidth + ImGui::GetStyle().FramePadding.y);

				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.1f, 0.15f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.2f, 0.2f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.8f, 0.1f, 0.15f, 1.0f));
				ImGui::PushID(("##" + valName + "ButtonX").c_str());
				if (ImGui::Button("X", buttonSize))
				{
					x = reset_x;
					returnVal != true;
				}
				ImGui::PopID();
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();

				ImGui::SetNextItemWidth(boxWidth);
				ImGui::SameLine();
				returnVal |= ImGui::DragFloat(("##" + valName + "X").c_str(), &x, 0.1f);

				ImGui::SameLine();
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
				ImGui::PushID(("##" + valName + "ButtonY").c_str());
				if (ImGui::Button("Y", buttonSize))
				{
					y = reset_y;
					returnVal != true;
				}
				ImGui::PopID();
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();

				ImGui::SetNextItemWidth(boxWidth);
				ImGui::SameLine();
				returnVal |= ImGui::DragFloat(("##" + valName + "Y").c_str(), &y, 0.1f);

				return returnVal;
			}

			bool Vector3_2Col(std::string valName, float& x, float& y, float& z, float reset_x, float reset_y, float reset_z)
			{
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				imgui::SmartStyleColor textColor(ImGuiCol_Text, IM_COL32(180, 180, 180, 255));
				ImGui::AlignTextToFramePadding();
				ImGui::Text(valName.c_str());
				ImGui::TableSetColumnIndex(1);
				
				imgui::SmartStyleVar itemPadding = imgui::SmartStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(1, 2));

				bool returnVal = false;

				const float rowWidth = ImGui::GetFontSize() + 4;
				const float boxWidth = ImGui::GetColumnWidth() / 3 - rowWidth - 2;
				const ImVec2 buttonSize = ImVec2(rowWidth, rowWidth + ImGui::GetStyle().FramePadding.y);

				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.1f, 0.15f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.2f, 0.2f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.8f, 0.1f, 0.15f, 1.0f));
				ImGui::PushID(("##" + valName + "ButtonX").c_str());
				if (ImGui::Button("X", buttonSize))
				{
					x = reset_x;
					returnVal != true;
				}
				ImGui::PopID();
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();

				ImGui::SetNextItemWidth(boxWidth);
				ImGui::SameLine();
				returnVal |= ImGui::DragFloat(("##" + valName + "X").c_str(), &x, 0.1f);

				ImGui::SameLine();
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
				ImGui::PushID(("##" + valName + "ButtonY").c_str());
				if (ImGui::Button("Y", buttonSize))
				{
					y = reset_y;
					returnVal != true;
				}
				ImGui::PopID();
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();

				ImGui::SetNextItemWidth(boxWidth);
				ImGui::SameLine();
				returnVal |= ImGui::DragFloat(("##" + valName + "Y").c_str(), &y, 0.1f);

				ImGui::SameLine();
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.25f, 0.8f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.35f, 0.9f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.25f, 0.8f, 1.0f));
				ImGui::PushID(("##" + valName + "ButtonZ").c_str());
				if (ImGui::Button("Z", buttonSize))
				{
					z = reset_z;
					returnVal != true;
				}
				ImGui::PopID();
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();

				ImGui::SetNextItemWidth(boxWidth);
				ImGui::SameLine();
				returnVal |= ImGui::DragFloat(("##" + valName + "Z").c_str(), &z, 0.1f);

				return returnVal;
			}

			bool Vector4_2Col(std::string valName, float& x, float& y, float& z, float& w, float reset_x, float reset_y, float reset_z, float reset_w)
			{
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				imgui::SmartStyleColor textColor(ImGuiCol_Text, IM_COL32(180, 180, 180, 255));
				ImGui::AlignTextToFramePadding();
				ImGui::Text(valName.c_str());
				ImGui::TableSetColumnIndex(1);

				imgui::SmartStyleVar itemPadding = imgui::SmartStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(1, 2));

				bool returnVal = false;

				const float rowWidth = ImGui::GetFontSize() + 4;
				const float boxWidth = ImGui::GetColumnWidth() / 4 - rowWidth - 2;
				const ImVec2 buttonSize = ImVec2(rowWidth, rowWidth + ImGui::GetStyle().FramePadding.y);

				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.1f, 0.15f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.2f, 0.2f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.8f, 0.1f, 0.15f, 1.0f));
				ImGui::PushID(("##" + valName + "ButtonX").c_str());
				if (ImGui::Button("X", buttonSize))
				{
					x = reset_x;
					returnVal != true;
				}
				ImGui::PopID();
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();

				ImGui::SetNextItemWidth(boxWidth);
				ImGui::SameLine();
				returnVal |= ImGui::DragFloat(("##" + valName + "X").c_str(), &x, 0.1f);

				ImGui::SameLine();
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
				ImGui::PushID(("##" + valName + "ButtonY").c_str());
				if (ImGui::Button("Y", buttonSize))
				{
					y = reset_y;
					returnVal != true;
				}
				ImGui::PopID();
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();

				ImGui::SetNextItemWidth(boxWidth);
				ImGui::SameLine();
				returnVal |= ImGui::DragFloat(("##" + valName + "Y").c_str(), &y, 0.1f);

				ImGui::SameLine();
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.25f, 0.8f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.35f, 0.9f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.25f, 0.8f, 1.0f));
				ImGui::PushID(("##" + valName + "ButtonZ").c_str());
				if (ImGui::Button("Z", buttonSize))
				{
					z = reset_z;
					returnVal != true;
				}
				ImGui::PopID();
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();

				ImGui::SetNextItemWidth(boxWidth);
				ImGui::SameLine();
				returnVal |= ImGui::DragFloat(("##" + valName + "Z").c_str(), &z, 0.1f);

				ImGui::SameLine();
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
				ImGui::PushID(("##" + valName + "ButtonW").c_str());
				if (ImGui::Button("W", buttonSize))
				{
					w = reset_w;
					returnVal != true;
				}
				ImGui::PopID();
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();

				ImGui::SetNextItemWidth(boxWidth);
				ImGui::SameLine();
				returnVal |= ImGui::DragFloat(("##" + valName + "W").c_str(), &w, 0.1f);

				return returnVal;
			}

			DragFlags DragFloatLabel(std::string dragKey, std::string label, float& value, bool editable, float speed, float min, float max)
			{
				using namespace ImGui;
				DragFlags returnVal = DragFlags::None;
				ImGuiWindow* window = GetCurrentWindow();
				if (window->SkipItems)
					return returnVal;

				ImGuiContext& g = *GImGui;
				const ImGuiStyle& style = g.Style;
				const ImGuiID id = window->GetID(dragKey.c_str());
				const float w = CalcItemWidth();

				const ImVec2 label_size = CalcTextSize(label.c_str(), NULL, true);
				const ImRect frame_bb(window->DC.CursorPos, ImVec2(window->DC.CursorPos.x + w, window->DC.CursorPos.y + label_size.y + style.FramePadding.y * 2.0f));
				const ImRect total_bb(frame_bb.Min, frame_bb.Max);

				bool temp_input_allowed = editable;
				ItemSize(total_bb, style.FramePadding.y);
				if (!ItemAdd(total_bb, id, &frame_bb, temp_input_allowed ? ImGuiItemFlags_Inputable : 0))
					return returnVal;

				// Default format string when passing NULL
				auto format = "%.3f";

				const bool hovered = ItemHoverable(frame_bb, id, g.LastItemData.InFlags);
				bool temp_input_is_active = TempInputIsActive(id);
				if (!temp_input_is_active)
				{
					// Tabbing or CTRL-clicking on Drag turns it into an InputText
					const bool input_requested_by_tabbing = (g.LastItemData.StatusFlags & ImGuiItemStatusFlags_FocusedByTabbing) != 0;
					const bool clicked = hovered && IsMouseClicked(0, id);
					const bool double_clicked = (hovered && g.IO.MouseClickedCount[0] == 2 && TestKeyOwner(ImGuiKey_MouseLeft, id));
					const bool make_active = (input_requested_by_tabbing || clicked || double_clicked || g.NavActivateId == id);
					if (make_active && (clicked || double_clicked))
						SetKeyOwner(ImGuiKey_MouseLeft, id);
					if (make_active)
						if (input_requested_by_tabbing || (clicked && g.IO.KeyCtrl) || double_clicked || (g.NavActivateId == id && (g.NavActivateFlags & ImGuiActivateFlags_PreferInput)))
							temp_input_is_active = true;

					// (Optional) simple click (without moving) turns Drag into an InputText
					if (g.IO.ConfigDragClickToInputText && !temp_input_is_active)
						if (g.ActiveId == id && hovered && g.IO.MouseReleased[0] && !IsMouseDragPastThreshold(0, g.IO.MouseDragThreshold * DRAG_MOUSE_THRESHOLD_FACTOR))
						{
							g.NavActivateId = id;
							g.NavActivateFlags = ImGuiActivateFlags_PreferInput;
							temp_input_is_active = true;
						}

					if (make_active && !temp_input_is_active)
					{
						SetActiveID(id, window);
						SetFocusID(id, window);
						FocusWindow(window);
						g.ActiveIdUsingNavDirMask = (1 << ImGuiDir_Left) | (1 << ImGuiDir_Right);
					}
				}

				if (temp_input_is_active)
				{
					TempInputScalar(frame_bb, id, dragKey.c_str(), ImGuiDataType_Float, &value, format, NULL, NULL);
					return DragFlags::Type;
				}

				// Draw frame
				const ImU32 frame_col = GetColorU32(g.ActiveId == id ? ImGuiCol_FrameBgActive : hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
				RenderNavHighlight(frame_bb, id);
				RenderFrame(frame_bb.Min, frame_bb.Max, frame_col, true, style.FrameRounding);

				// Drag behavior
				const bool value_changed = DragBehavior(id, ImGuiDataType_Float, &value, speed, &min, &max, format, 0);
				if (value_changed)
				{
					MarkItemEdited(id);
					returnVal = DragFlags::Drag;
				}

				// Display value using user-provided display format so user can add prefix/suffix/decorations to the value.
				if (g.LogEnabled)
					LogSetNextTextDecoration("{", "}");
				RenderTextClipped(frame_bb.Min, frame_bb.Max, label.c_str(), label.c_str() + label.size(), NULL, ImVec2(0.5f, 0.5f));
	
				return returnVal;
			}

			DragFlags DragIntLabel(std::string dragKey, std::string label, int& value, bool editable, float speed, int min, int max)
			{
				using namespace ImGui;
				DragFlags returnVal = DragFlags::None;
				ImGuiWindow* window = GetCurrentWindow();
				if (window->SkipItems)
					return returnVal;

				ImGuiContext& g = *GImGui;
				const ImGuiStyle& style = g.Style;
				const ImGuiID id = window->GetID(dragKey.c_str());
				const float w = CalcItemWidth();

				const ImVec2 label_size = CalcTextSize(label.c_str(), NULL, true);
				const ImRect frame_bb(window->DC.CursorPos, ImVec2(window->DC.CursorPos.x + w, window->DC.CursorPos.y + label_size.y + style.FramePadding.y * 2.0f));
				const ImRect total_bb(frame_bb.Min, frame_bb.Max);

				bool temp_input_allowed = editable;
				ItemSize(total_bb, style.FramePadding.y);
				if (!ItemAdd(total_bb, id, &frame_bb, temp_input_allowed ? ImGuiItemFlags_Inputable : 0))
					return returnVal;

				// Default format string when passing NULL
				auto format = "%d";

				const bool hovered = ItemHoverable(frame_bb, id, g.LastItemData.InFlags);
				bool temp_input_is_active = TempInputIsActive(id);
				if (!temp_input_is_active)
				{
					// Tabbing or CTRL-clicking on Drag turns it into an InputText
					const bool input_requested_by_tabbing = (g.LastItemData.StatusFlags & ImGuiItemStatusFlags_FocusedByTabbing) != 0;
					const bool clicked = hovered && IsMouseClicked(0, id);
					const bool double_clicked = (hovered && g.IO.MouseClickedCount[0] == 2 && TestKeyOwner(ImGuiKey_MouseLeft, id));
					const bool make_active = (input_requested_by_tabbing || clicked || double_clicked || g.NavActivateId == id);
					if (make_active && (clicked || double_clicked))
						SetKeyOwner(ImGuiKey_MouseLeft, id);
					if (make_active)
						if (input_requested_by_tabbing || (clicked && g.IO.KeyCtrl) || double_clicked || (g.NavActivateId == id && (g.NavActivateFlags & ImGuiActivateFlags_PreferInput)))
							temp_input_is_active = true;

					// (Optional) simple click (without moving) turns Drag into an InputText
					if (g.IO.ConfigDragClickToInputText && !temp_input_is_active)
						if (g.ActiveId == id && hovered && g.IO.MouseReleased[0] && !IsMouseDragPastThreshold(0, g.IO.MouseDragThreshold * DRAG_MOUSE_THRESHOLD_FACTOR))
						{
							g.NavActivateId = id;
							g.NavActivateFlags = ImGuiActivateFlags_PreferInput;
							temp_input_is_active = true;
						}

					if (make_active && !temp_input_is_active)
					{
						SetActiveID(id, window);
						SetFocusID(id, window);
						FocusWindow(window);
						g.ActiveIdUsingNavDirMask = (1 << ImGuiDir_Left) | (1 << ImGuiDir_Right);
					}
				}

				if (temp_input_is_active)
				{
					TempInputScalar(frame_bb, id, dragKey.c_str(), ImGuiDataType_S32, &value, format, NULL, NULL);
					return DragFlags::Type;
				}

				// Draw frame
				const ImU32 frame_col = GetColorU32(g.ActiveId == id ? ImGuiCol_FrameBgActive : hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
				RenderNavHighlight(frame_bb, id);
				RenderFrame(frame_bb.Min, frame_bb.Max, frame_col, true, style.FrameRounding);

				// Drag behavior
				const bool value_changed = DragBehavior(id, ImGuiDataType_S32, &value, speed, &min, &max, format, 0);
				if (value_changed)
				{
					MarkItemEdited(id);
					returnVal = DragFlags::Drag;
				}

				// Display value using user-provided display format so user can add prefix/suffix/decorations to the value.
				if (g.LogEnabled)
					LogSetNextTextDecoration("{", "}");
				RenderTextClipped(frame_bb.Min, frame_bb.Max, label.c_str(), label.c_str() + label.size(), NULL, ImVec2(0.5f, 0.5f));

				return returnVal;
			}

			Vector3Flags Vector3Control(std::string valName, float& x, float& y, float& z, bool x_multiSelect, bool y_multiSelect, bool z_multiSelect)
			{
				Vector3Flags returnVal = Vector3Flags::None;
				if (ImGui::BeginTable("##TransFormComponent", 2, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_NoClip))
				{
					ImGui::TableSetupColumn("label_column", 0, 60);
					ImGui::TableSetupColumn("value_column", ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_NoClip, ImGui::GetContentRegionAvail().x - 60);

					imgui::SmartStyleVar itemPadding = imgui::SmartStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(1, 2));

					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					ImGui::Text(valName.c_str());
					ImGui::TableSetColumnIndex(1);

					const float rowWidth = ImGui::GetFontSize() + 4;
					const float boxWidth = ImGui::GetColumnWidth() / 3 - rowWidth - 2;
					const ImVec2 buttonSize = ImVec2(rowWidth, rowWidth + ImGui::GetStyle().FramePadding.y);

					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.1f, 0.15f, 1.0f));
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.2f, 0.2f, 1.0f));
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.8f, 0.1f, 0.15f, 1.0f));
					if (ImGui::Button("X", buttonSize))
					{
						returnVal = Vector3Flags::ResetX;
					}
					ImGui::PopStyleColor();
					ImGui::PopStyleColor();
					ImGui::PopStyleColor();

					ImGui::SetNextItemWidth(boxWidth);
					ImGui::SameLine();

					static bool editFlagX = false;
					static bool editTypeX = false;
					if (x_multiSelect)
					{
						auto flag = DragFloatLabel(("##" + valName + "multiX").c_str(), "---", x, 0.1f);
						switch (flag)
						{
							case application::editor::imgui::DragFlags::Drag:
							{
								editFlagX = true;
								editTypeX = false;
								break;
							}
							case application::editor::imgui::DragFlags::Type:
							{
								editFlagX = true;
								editTypeX = true;
								returnVal = Vector3Flags::TypeX;
								break;
							}
							default:
								break;
						}
					}
					else
					{
						if (ImGui::DragFloat(("##" + valName + "X").c_str(), &x, 0.1f))
						{
							editFlagX = true;
							editTypeX = false;
						}
					}

					if (editFlagX)
					{
						if (ImGui::GetFocusID() != ImGui::GetActiveID())
						{
							if (editTypeX)
							{
								returnVal = Vector3Flags::EndTypeX;
							}
							else
							{
								returnVal = Vector3Flags::EndEditX;
							}
							editFlagX = false;
							editTypeX = false;
						}
					}

					ImGui::SameLine();
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
					if (ImGui::Button("Y", buttonSize))
					{
						returnVal = Vector3Flags::ResetY;
					}
					ImGui::PopStyleColor();
					ImGui::PopStyleColor();
					ImGui::PopStyleColor();

					ImGui::SetNextItemWidth(boxWidth);
					ImGui::SameLine();

					static bool editFlagY = false;
					static bool editTypeY = false;
					if (y_multiSelect)
					{
						auto flag = DragFloatLabel(("##" + valName + "multiY").c_str(), "---", y, 0.1f);
						switch (flag)
						{
							case application::editor::imgui::DragFlags::Drag:
							{
								editFlagY = true;
								editTypeY = false;
								break;
							}
							case application::editor::imgui::DragFlags::Type:
							{
								editFlagY = true;
								editTypeY = true;
								returnVal = Vector3Flags::TypeY;
								break;
							}
							default:
								break;
						}
					}
					else
					{
						if (ImGui::DragFloat(("##" + valName + "Y").c_str(), &y, 0.1f))
						{
							editFlagY = true;
							editTypeY = false;
						}
					}

					if (editFlagY)
					{
						if (ImGui::GetFocusID() != ImGui::GetActiveID())
						{
							if (editTypeY)
							{
								returnVal = Vector3Flags::EndTypeY;
							}
							else
							{
								returnVal = Vector3Flags::EndEditY;
							}
							editFlagY = false;
							editTypeY = false;
						}
					}

					ImGui::SameLine();
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.25f, 0.8f, 1.0f));
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.35f, 0.9f, 1.0f));
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.25f, 0.8f, 1.0f));
					if (ImGui::Button("Z", buttonSize))
					{
						returnVal = Vector3Flags::ResetZ;
					}
					ImGui::PopStyleColor();
					ImGui::PopStyleColor();
					ImGui::PopStyleColor();

					ImGui::SetNextItemWidth(boxWidth);
					ImGui::SameLine();

					static bool editFlagZ = false;
					static bool editTypeZ = false;
					if (z_multiSelect)
					{
						auto flag = DragFloatLabel(("##" + valName + "multiZ").c_str(), "---", z, 0.1f);
						switch (flag)
						{
							case application::editor::imgui::DragFlags::Drag:
							{
								editFlagZ = true;
								editTypeZ = false;
								break;
							}
							case application::editor::imgui::DragFlags::Type:
							{
								editFlagZ = true;
								editTypeZ = true;
								returnVal = Vector3Flags::TypeZ;
								break;
							}
							default:
								break;
						}
					}
					else
					{
						if (ImGui::DragFloat(("##" + valName + "Z").c_str(), &z, 0.1f))
						{
							editFlagZ = true;
							editTypeZ = false;
						}
					}

					if (editFlagZ)
					{
						if (ImGui::GetFocusID() != ImGui::GetActiveID())
						{
							if (editTypeZ)
							{
								returnVal = Vector3Flags::EndTypeZ;
							}
							else
							{
								returnVal = Vector3Flags::EndEditZ;
							}
							editFlagZ = false;
							editTypeZ = false;
						}
					}

					ImGui::EndTable();
				}
				return returnVal;
			}

			void EndSection()
			{
				ImGui::EndTable();
			}

			bool IsItemHovered(float delayInSeconds, ImGuiHoveredFlags flags)
			{
				return ImGui::IsItemHovered() && GImGui->HoveredIdTimer > delayInSeconds; /*HoveredIdNotActiveTimer*/
			}

			void SetTooltip(std::string tooltip, float delayInSeconds, bool allowWhenDisabled, ImVec2 padding)
			{
				if (IsItemHovered(delayInSeconds, allowWhenDisabled ? ImGuiHoveredFlags_AllowWhenDisabled : 0))
				{
					imgui::SmartStyleVar tooltipPadding(ImGuiStyleVar_WindowPadding, padding);
					imgui::SmartStyleColor textCol(ImGuiCol_Text, IM_COL32(210, 210, 210, 255));
					ImGui::SetTooltip(tooltip.c_str());
				}
			}

			std::unordered_map<std::string, MessageBoxData> s_MessageBoxes = std::unordered_map<std::string, MessageBoxData>();

			void ShowMessageBox(std::string title, const std::function<void()>& renderFunction, uint32_t width, uint32_t height)
			{
				auto& messageBoxData = s_MessageBoxes[title];
				messageBoxData.Title = title;
				messageBoxData.UserRenderFunction = renderFunction;
				messageBoxData.Flags = MESSAGE_BOX_USER_FUNC;
				messageBoxData.Width = width;
				messageBoxData.Height = height;
				messageBoxData.ShouldOpen = true;
			}

			void CloseMessageBox(std::string title)
			{
				if (s_MessageBoxes.find(title) != s_MessageBoxes.end())
				{
					s_MessageBoxes[title].IsOpen = false;
				}
			}

			void RenderMessageBoxes()
			{
				for (auto& each : s_MessageBoxes)
				{
					RenderMessageBox(each.first);
				}
			}

			void RenderMessageBox(std::string title)
			{
				auto& messageBoxData = s_MessageBoxes.at(title);

				if (messageBoxData.ShouldOpen && !ImGui::IsPopupOpen(nullptr, ImGuiPopupFlags_AnyPopupId))
				{
					ImGui::OpenPopup(messageBoxData.Title.c_str());
					messageBoxData.ShouldOpen = false;
					messageBoxData.IsOpen = true;
				}

				if (!messageBoxData.IsOpen)
				{
					return;
				}

				ImVec2 center = ImGui::GetMainViewport()->GetCenter();
				ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
				ImGui::SetNextWindowSize(ImVec2((float)messageBoxData.Width, (float)messageBoxData.Height));

				if (ImGui::BeginPopupModal(messageBoxData.Title.c_str(), &messageBoxData.IsOpen, ImGuiWindowFlags_AlwaysAutoResize))
				{
					if (messageBoxData.Flags & MESSAGE_BOX_USER_FUNC)
					{
						messageBoxData.UserRenderFunction();
					}
					else
					{
						ImGui::TextWrapped(messageBoxData.Body.c_str());

						if (messageBoxData.Flags & MESSAGE_BOX_OK_BUTTON)
						{
							if (ImGui::Button("Ok"))
							{
								ImGui::CloseCurrentPopup();
								messageBoxData.IsOpen = false;
							}

							if (messageBoxData.Flags & MESSAGE_BOX_CANCEL_BUTTON)
							{
								ImGui::SameLine();
							}
						}

						if (messageBoxData.Flags & MESSAGE_BOX_CANCEL_BUTTON && ImGui::Button("Cancel"))
						{
							ImGui::CloseCurrentPopup();
							messageBoxData.IsOpen = false;
						}
					}

					ImGui::EndPopup();
				}
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
