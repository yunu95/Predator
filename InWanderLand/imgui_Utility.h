/// 2024. 01. 18 김상준
/// ImGui 에서 사용할 수 있는 기능들을 모아서 정리한 Utility 파일

#pragma once

#include "imgui.h"
#include "imgui_internal.h"

#include <string>

namespace application
{
	namespace editor
	{
		namespace imgui
		{
			// x, y 크기만큼 확장한 ImRect 획득
			ImRect RectExpanded(const ImRect& rect, float x, float y);

			bool IsItemHovered(float delayInSeconds = 0.1f, ImGuiHoveredFlags flags = 0);

			void SetTooltip(std::string text, float delayInSeconds = 0.1f, bool allowWhenDisabled = true, ImVec2 padding = ImVec2(5, 5));

			void ShiftCursorX(float distance);

			void ShiftCursorY(float distance);

			void ShiftCursor(float x, float y);

			// Push / Pop 을 신경쓰지 않고 소멸자에서 Pop 을 수행하는 StyleVar객체
			class SmartStyleVar
			{
			public:
				template<typename T>
				SmartStyleVar(ImGuiStyleVar styleVar, T value) { ImGui::PushStyleVar(styleVar, value); }
				~SmartStyleVar() { ImGui::PopStyleVar(); }

				SmartStyleVar(const SmartStyleVar&) = delete;
				SmartStyleVar& operator=(const SmartStyleVar&) = delete;
			};

			// Push / Pop 을 신경쓰지 않고 소멸자에서 Pop 을 수행하는 StyleColor객체
			class SmartStyleColor
			{
			public:
				SmartStyleColor(const SmartStyleColor&) = delete;
				SmartStyleColor& operator=(const SmartStyleColor&) = delete;
				template<typename T>
				SmartStyleColor(ImGuiCol colourId, T colour) { ImGui::PushStyleColor(colourId, ImColor(colour).Value); }
				~SmartStyleColor() { ImGui::PopStyleColor(); }
			};

			namespace draw
			{
				void Underline(ImColor color = IM_COL32(26, 26, 26, 255), bool fullWidth = false, float offsetX = 0.0f, float offsetY = -1.0f);
			}
		}
	}
}