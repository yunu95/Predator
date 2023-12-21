#include "Panel_Palette.h"

#include "imgui.h"

namespace application
{
	namespace editor
	{
		PalettePanel::PalettePanel()
		{

		}

		PalettePanel::~PalettePanel()
		{

		}

		void PalettePanel::Initialize()
		{

		}

		void PalettePanel::Update(float ts)
		{

		}

		void PalettePanel::GUIProgress()
		{
			ImGui::Begin("Palette");

			/// ImGui 관련 내부 변수 업데이트
			isMouseOver = ImGui::IsWindowHovered();
			isFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);
			
			/// 실제 패널에 그리는 영역


			ImGui::End();
		}

		void PalettePanel::Finalize()
		{

		}
	}
}
