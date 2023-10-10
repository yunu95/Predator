#include "Panel_GameView.h"

#include "imgui.h"

namespace Editor
{
	GameViewPanel* GameViewPanel::instance = nullptr;

	GameViewPanel& GameViewPanel::GetInstance()
	{
		if (instance == nullptr)
		{
			instance = new GameViewPanel();
		}

		return *instance;
	}

	GameViewPanel::~GameViewPanel()
	{
		delete instance;
	}

	void GameViewPanel::Initialize()
	{

	}

	void GameViewPanel::Update(float ts)
	{

	}

	void GameViewPanel::DrawPanel()
	{
		ImGui::Begin("GameView");
		
		/// ImGui 관련 내부 변수 업데이트
		isMouseOver = ImGui::IsWindowHovered();
		isFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);
		
		/// 실제 패널에 그리는 영역


		ImGui::End();
	}
	
	void GameViewPanel::Finalize()
	{

	}	

	GameViewPanel::GameViewPanel()
	{

	}
}
