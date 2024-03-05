/// 2024. 01. 18 김상준
/// ImGui 에서 사용할 수 있는 기능들을 모아서 정리한 Utility 파일

#pragma once

#include "Storable.h"
#include "ITemplateData.h"
#include "TemplateDataList.h"

#include "imgui.h"
#include "imgui_internal.h"

#include <string>
#include <functional>

namespace application
{
	namespace editor
	{
		namespace imgui
		{
			enum class Vector3Flags
			{
				None,
				ResetX,
				ResetY,
				ResetZ,
				TypeX,
				TypeY,
				TypeZ
			};

			// x, y 크기만큼 확장한 ImRect 획득
			ImRect RectExpanded(const ImRect& rect, float x, float y);

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

			bool SelectableImageButton(std::string imageKey, std::string filename, bool selected, const ImVec2& size, const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1, 1), bool useTint = false, const ImVec4& bgColor = ImVec4(0.26f, 0.59f, 0.98f, 0.31f), const ImVec4& selectedColor = ImVec4(0.26f, 0.59f, 0.98f, 1.00f));

#pragma region 1Column
			bool BeginSection_1Col(int& countIdx, std::string sectionName, float width, ImColor underlineColor = IM_COL32(90, 90, 90, 200));
#pragma endregion

#pragma region 2Column
			bool BeginSection_2Col(int& countIdx, std::string sectionName, float width, float firstColRatio = 0.5f, ImColor underlineColor = IM_COL32(90, 90, 90, 200));
			bool SliderFloat_2Col(std::string valName, float& value, float min = 0.0f, float max = 0.0f);
			bool SliderInt_2Col(std::string valName, int& value, int min = 0, int max = 0);
			bool DragFloat_2Col(std::string valName, float& value, float speed = 1.0f, float min = 0.0f, float max = 0.0f);
			bool DragInt_2Col(std::string valName, int& value, float speed = 1.0f, int min = 0, int max = 0);
			bool Checkbox_2Col(std::string valName, bool& value);
			bool Dropdown_2Col(std::string valName, const char** options, int optionCount, int* selected);
			bool Vector2_2Col(std::string valName, float& x, float& y, float reset_x = 0.0f, float reset_y = 0.0f);
			bool Vector3_2Col(std::string valName, float& x, float& y, float& z, float reset_x = 0.0f, float reset_y = 0.0f, float reset_z = 0.0f);
			bool Vector4_2Col(std::string valName, float& x, float& y, float& z, float& w, float reset_x = 0.0f, float reset_y = 0.0f, float reset_z = 0.0f, float reset_w = 1.0f);
#pragma endregion

			// 여기서 bool은 직접 값을 넣어서 변경했는지에 대한 여부를 반환함
			bool DragFloatLabel(std::string dragKey, std::string label, float& value, float speed = 1.0f, float min = 0.0f, float max = 0.0f);
			// 여기서 bool은 직접 값을 넣어서 변경했는지에 대한 여부를 반환함
			bool DragIntLabel(std::string dragKey, std::string label, int& value, float speed = 1.0f, int min = 0, int max = 0);
			Vector3Flags Vector3Control(std::string valName, float& x, float& y, float& z, bool x_multiSelect = false, bool y_multiSelect = false, bool z_multiSelect = false);

			void EndSection();

			bool IsItemHovered(float delayInSeconds = 0.3f, ImGuiHoveredFlags flags = 0);
			void SetTooltip(std::string tooltip, float delayInSeconds = 0.3f, bool allowWhenDisabled = true, ImVec2 padding = ImVec2(5, 5));

			namespace draw
			{
				void Underline(ImColor color = IM_COL32(26, 26, 26, 255), bool fullWidth = false, float offsetX = 0.0f, float offsetY = -1.0f);
			}
		}
	}
}