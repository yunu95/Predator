#include "Panel_SceneView.h"

#include "imgui.h"

#include "YunutyEngine.h"

#include <d3d11.h>

namespace application
{
	namespace editor
	{
		SceneViewPanel::SceneViewPanel()
			: rendererWidth(), rendererHeight(), resourceManager(nullptr)
		{

		}

		SceneViewPanel::~SceneViewPanel()
		{

		}

		void SceneViewPanel::Initialize()
		{
			auto rect = yunutyEngine::graphics::Renderer::SingleInstance().GetResolution();
			rendererWidth = rect.x;
			rendererHeight = rect.y;
			resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
		}

		void SceneViewPanel::Update(float ts)
		{

		}

		void SceneViewPanel::GUIProgress()
		{
			ImGui::Begin("SceneView", 0, ImGuiWindowFlags_NoBringToFrontOnFocus);

			/// ImGui 관련 내부 변수 업데이트
			isMouseOver = ImGui::IsWindowHovered();
			isFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);

			/// 실제 패널에 그리는 영역
			ID3D11ShaderResourceView* sceneImage = static_cast<ID3D11ShaderResourceView*>(resourceManager->GetFinalRenderImage());

			ImGui::Image((void*)sceneImage, ImVec2(rendererWidth, rendererHeight));

			ImGui::End();
		}

		void SceneViewPanel::Finalize()
		{

		}
	}
}
