#include "Panel_Hierarchy.h"

#include "imgui.h"

namespace Application
{
	namespace Editor
	{
		std::unique_ptr<HierarchyPanel> HierarchyPanel::instance = nullptr;

		HierarchyPanel& HierarchyPanel::GetInstance()
		{
			if (instance == nullptr)
			{
				instance = std::unique_ptr<HierarchyPanel>(new HierarchyPanel());
			}

			return *instance;
		}

		HierarchyPanel::~HierarchyPanel()
		{
		
		}

		void HierarchyPanel::Initialize()
		{

		}

		void HierarchyPanel::Update(float ts)
		{

		}

		void HierarchyPanel::GUIProgress()
		{
			ImGui::Begin("Hierarchy");

			/// ImGui 관련 내부 변수 업데이트
			isMouseOver = ImGui::IsWindowHovered();
			isFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);

			/// 실제 패널에 그리는 영역


			ImGui::End();
		}

		void HierarchyPanel::Finalize()
		{

		}

		HierarchyPanel::HierarchyPanel()
		{

		}
	}
}
