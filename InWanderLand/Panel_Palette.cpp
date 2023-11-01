#include "Panel_Palette.h"

#include "imgui.h"

namespace Application
{
	namespace Editor
	{
		std::unique_ptr<PalettePanel> PalettePanel::instance = nullptr;

		PalettePanel& PalettePanel::GetInstance()
		{
			if (instance == nullptr)
			{
				instance = std::unique_ptr<PalettePanel>(new PalettePanel());
			}

			return *instance;
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

		PalettePanel::PalettePanel()
		{

		}
	}
}
