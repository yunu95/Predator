#include "Panel_SceneView.h"

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

#include "Application.h"
#include "YunutyEngine.h"

#include "WindowEvents.h"
#include "MouseEvents.h"

#include "UnitPaletteManager.h"

#include <d3d11.h>

namespace application
{
	namespace editor
	{
		SceneViewPanel::SceneViewPanel()
			: app(nullptr), renderImageSize(), cursorPos_InScreenSpace(), resize(false), mouseMove(false)
		{

		}

		SceneViewPanel::~SceneViewPanel()
		{

		}

		void SceneViewPanel::Initialize()
		{
			app = &Application::GetInstance();
		}

		void SceneViewPanel::Update(float ts)
		{

		}

		void SceneViewPanel::GUIProgress()
		{
			ImGui::Begin("SceneView", 0, ImGuiWindowFlags_NoBringToFrontOnFocus);

			ImGui_Update();

			/// ImGui 관련 내부 변수 업데이트
			isMouseOver = ImGui::IsWindowHovered();
			isFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);

			/// 실제 패널에 그리는 영역
			ImGui::Image(reinterpret_cast<ImTextureID>(app->GetSceneSRV()), ImVec2(renderImageSize.first, renderImageSize.second));

			ImGui::End();
		}

		void SceneViewPanel::Finalize()
		{

		}

		void SceneViewPanel::OnEvent(EditorEvents& event)
		{
			EventDispatcher dispatcher(event);
			dispatcher.Dispatch<WindowResizeEvent>([&, this](WindowResizeEvent& e) { resize = true; return true; });

			if (isFocused)
			{
				if (isMouseOver)
				{
					/// Test
					static auto& upm = palette::UnitPaletteManager::SingleInstance();

					dispatcher.Dispatch<MouseButtonPressedEvent>([&, this](MouseButtonPressedEvent& e) { upm.OnLeftClick(); return true; });
					dispatcher.Dispatch<MouseMoveEvent>([&, this](MouseMoveEvent& e) { mouseMove = true; return true; });
					dispatcher.Dispatch<MouseButtonUpEvent>([&, this](MouseButtonUpEvent& e) { upm.OnLeftClickRelease(); return true; });
				}
			}
		}

		std::pair<unsigned int, unsigned int> SceneViewPanel::GetRenderImageSize() const
		{
			return renderImageSize;
		}

		std::pair<double, double> SceneViewPanel::GetCursorPosInScreenSpace() const
		{
			return cursorPos_InScreenSpace;
		}

		void SceneViewPanel::ImGui_Update()
		{
			/// 임시
			static auto& upm = palette::UnitPaletteManager::SingleInstance();
			///

			if (resize)
			{
				ImGui_OnResizeRenderImageSize();
				resize = false;
			}

			if (mouseMove)
			{
				if (ImGui_IsCursorInScreen())
				{
					ImGui_UpdateCursorPosInScreenSpace();
					auto distToXZPlane = abs(yunutyEngine::graphics::Camera::GetMainCamera()->GetTransform()->GetWorldPosition().y);
					auto projectedPos = yunutyEngine::graphics::Camera::GetMainCamera()->GetProjectedPoint({ cursorPos_InScreenSpace.first, cursorPos_InScreenSpace.second }, distToXZPlane);
					upm.OnMouseMove(projectedPos);
				}
				mouseMove = false;
			}
		}

		void SceneViewPanel::ImGui_OnResizeRenderImageSize()
		{
			// 그려지는 영역에 맞게 화면 비 재구성
			auto rect = yunutyEngine::graphics::Renderer::SingleInstance().GetResolution();
			float ratio = (float)rect.y / (float)rect.x;
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

			renderImageSize.first = newRegion.x;
			renderImageSize.second = newRegion.y;
		}

		void SceneViewPanel::ImGui_UpdateCursorPosInScreenSpace()
		{
			auto curPos = ImGui_GetCursorPosOnPanel();
			ImVec2 startPos = ImGui::GetCursorPos();

			ImVec2 finalPos(curPos.first - startPos.x, curPos.second - startPos.y);

			cursorPos_InScreenSpace.first = (finalPos.x - (renderImageSize.first / 2)) / renderImageSize.first;
			cursorPos_InScreenSpace.second = (finalPos.y - (renderImageSize.second / 2)) / -renderImageSize.second;
		}

		bool SceneViewPanel::ImGui_IsCursorInScreen()
		{
			auto curPos = ImGui_GetCursorPosOnPanel();
			ImVec2 startPos = ImGui::GetCursorPos();
			return (curPos.first >= startPos.x && curPos.first <= startPos.x + renderImageSize.first) && (curPos.second >= startPos.y && curPos.second <= startPos.y + renderImageSize.second);
		}

		std::pair<float, float> SceneViewPanel::ImGui_GetCursorPosOnPanel()
		{
			auto winPos = ImGui::GetWindowPos();
			auto curPos = ImGui::GetMousePos();

			return std::pair<float, float>(curPos.x - winPos.x, curPos.y - winPos.y);
		}
	}
}
