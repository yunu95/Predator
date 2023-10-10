#include "EditorLayer.h"

#include "Panel/EditorPanel.h"
#include "Panel/Panel_Hierarchy.h"
#include "Panel/Panel_Inspector.h"
#include "Panel/Panel_Minimap.h"
#include "Panel/Panel_SceneView.h"
#include "Panel/Panel_GameView.h"

namespace Editor
{
	void EditorLayer::Initialize()
	{
		/// 에디터에서 사용할 에디터 카메라, 게임뷰 카메라 생성

		/// 에디터 패널 생성 및 초기화 진행
		editorPanelList.reserve(5);

		editorPanelList.push_back(&HierarchyPanel::GetInstance());
		editorPanelList.push_back(&InspectorPanel::GetInstance());
		editorPanelList.push_back(&MiniMapPanel::GetInstance());
		editorPanelList.push_back(&SceneViewPanel::GetInstance());
		editorPanelList.push_back(&GameViewPanel::GetInstance());

		for (auto each : editorPanelList)
		{
			each->Initialize();
		}
	}

	void EditorLayer::Update(float ts)
	{
		for (auto each : editorPanelList)
		{
			each->Update(ts);
		}
	}

	void EditorLayer::Render()
	{
		for (auto each : editorPanelList)
		{
			each->DrawPanel();
		}
	}

	void EditorLayer::Finalize()
	{
		for (auto each : editorPanelList)
		{
			each->Finalize();
		}
	}
}
