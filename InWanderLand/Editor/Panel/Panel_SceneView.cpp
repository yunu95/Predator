#include "Panel_SceneView.h"

#include "imgui.h"

namespace Editor
{
	SceneViewPanel* SceneViewPanel::instance = nullptr;

	SceneViewPanel& SceneViewPanel::GetInstance()
	{
		if (instance == nullptr)
		{
			instance = new SceneViewPanel();
		}

		return *instance;
	}

	SceneViewPanel::~SceneViewPanel()
	{
		delete instance;
	}

	void SceneViewPanel::Initialize()
	{

	}

	void SceneViewPanel::Update(float ts)
	{

	}

	void SceneViewPanel::DrawPanel()
	{
		ImGui::Begin("SceneView");
		
		/// ImGui 관련 내부 변수 업데이트
		isMouseOver = ImGui::IsWindowHovered();
		isFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);
		
		/// 실제 패널에 그리는 영역


		ImGui::End();
	}
	
	void SceneViewPanel::Finalize()
	{

	}	

	SceneViewPanel::SceneViewPanel()
	{

	}
}
