#include "Panel_MiniMap.h"

#include "imgui.h"

namespace Editor
{
	MiniMapPanel* MiniMapPanel::instance = nullptr;

	MiniMapPanel& MiniMapPanel::GetInstance()
	{
		if (instance == nullptr)
		{
			instance = new MiniMapPanel();
		}

		return *instance;
	}

	MiniMapPanel::~MiniMapPanel()
	{
		delete instance;
	}

	void MiniMapPanel::Initialize()
	{

	}

	void MiniMapPanel::Update(float ts)
	{

	}

	void MiniMapPanel::DrawPanel()
	{
		ImGui::Begin("MiniMap");
		
		/// ImGui 관련 내부 변수 업데이트
		isMouseOver = ImGui::IsWindowHovered();
		isFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);
		
		/// 실제 패널에 그리는 영역


		ImGui::End();
	}
	
	void MiniMapPanel::Finalize()
	{

	}	

	MiniMapPanel::MiniMapPanel()
	{

	}
}
