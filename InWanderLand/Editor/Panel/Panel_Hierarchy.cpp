#include "Panel_Hierarchy.h"

#include "imgui.h"

namespace Editor
{
	HierarchyPanel* HierarchyPanel::instance = nullptr;

	HierarchyPanel& HierarchyPanel::GetInstance()
	{
		if (instance == nullptr)
		{
			instance = new HierarchyPanel();
		}

		return *instance;
	}

	HierarchyPanel::~HierarchyPanel()
	{
		delete instance;
	}

	void HierarchyPanel::Initialize()
	{

	}

	void HierarchyPanel::Update(float ts)
	{

	}

	void HierarchyPanel::DrawPanel()
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
