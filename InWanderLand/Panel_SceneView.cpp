#include "InWanderLand.h"
#include "Panel_SceneView.h"

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

#include "Application.h"
#include "YunutyEngine.h"

#include <d3d11.h>

namespace application
{
	namespace editor
	{
		SceneViewPanel::SceneViewPanel()
			: app(nullptr), rendererWidth(), rendererHeight()
		{

		}

		SceneViewPanel::~SceneViewPanel()
		{

		}

		void SceneViewPanel::Initialize()
		{
			app = &Application::GetInstance();
			auto rect = yunutyEngine::graphics::Renderer::SingleInstance().GetResolution();
			rendererWidth = rect.x;
			rendererHeight = rect.y;
		}

		void SceneViewPanel::Update(float ts)
		{
			// Resize 이벤트에 따라서 rendererWidth, Height 바꾸기
		}

		void SceneViewPanel::GUIProgress()
		{
			ImGui::Begin("SceneView", 0, ImGuiWindowFlags_NoBringToFrontOnFocus);

			/// ImGui 관련 내부 변수 업데이트
			isMouseOver = ImGui::IsWindowHovered();
			isFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);

			/// 실제 패널에 그리는 영역

			// 그려지는 영역에 맞게 화면 비 재구성
			float ratio = (float)rendererHeight / (float)rendererWidth;
			ImVec2 newRegion = ImGui::GetContentRegionAvail();
			float newRegionRatio = newRegion.y / newRegion.x;

			if (newRegionRatio >= ratio)
			{
				newRegion.y = newRegion.x * ratio;
			}
			else
			{
				newRegion.x = newRegion.y / ratio;
			}

			ImGui::Image(reinterpret_cast<ImTextureID>(app->GetSceneSRV()), newRegion);

			ImGui::End();
		}

		void SceneViewPanel::Finalize()
		{

		}
	}
}
