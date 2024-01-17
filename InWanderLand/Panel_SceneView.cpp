#ifdef EDITOR
#include "InWanderLand.h"
#include "Panel_SceneView.h"

#include "imgui.h"
#include "imgui_internal.h"

#include "Application.h"
#include "YunutyEngine.h"

#include "WindowEvents.h"
#include "MouseEvents.h"

#include "UnitPaletteManager.h"

#include <d3d11.h>

#include <iostream>

namespace application
{
	namespace editor
	{
		SceneViewPanel::SceneViewPanel()
			: app(nullptr), prevWindowSize(), currentWindowSize(), renderImageSize(), cursorPos_InScreenSpace()
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
			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(1, 0, 1, 1));

			ImGui::Begin("SceneView", 0, ImGuiWindowFlags_NoBringToFrontOnFocus);

			ImGui_Update();

			/// 실제 패널에 그리는 영역
			ImGui::Image(reinterpret_cast<ImTextureID>(app->GetSceneSRV()), ImVec2(renderImageSize.first, renderImageSize.second));

			ImGui::End();

			ImGui::PopStyleColor();
		}

		void SceneViewPanel::Finalize()
		{

		}

		void SceneViewPanel::OnEvent(EditorEvents& event)
		{
			EventDispatcher dispatcher(event);


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
			/// ImGui 관련 내부 변수 업데이트
			isMouseOver = ImGui::IsWindowHovered();
			isFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);

			

			/// 임시
			static auto& upm = palette::UnitPaletteManager::SingleInstance();
			///

			ImGui_UpdateWindowSize();

			// Resize 에 대한 처리 부분
			if (ImGui_IsWindowResize())
			{
				ImGui_OnResizeRenderImageSize();
			}

			if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{
				upm.OnLeftClick();
			}

			

			// 입력에 대한 처리
			if (isFocused)
			{
				if (isMouseOver)
				{
					if (ImGui_IsCursorInScreen())
					{
						ImGui_UpdateCursorPosInScreenSpace();
						auto distToXZPlane = abs(yunutyEngine::graphics::Camera::GetMainCamera()->GetTransform()->GetWorldPosition().y);
						auto projectedPos = yunutyEngine::graphics::Camera::GetMainCamera()->GetProjectedPoint({ cursorPos_InScreenSpace.first, cursorPos_InScreenSpace.second }, distToXZPlane);
						upm.OnMouseMove(projectedPos);
					}
					else
					{
						Release();
					}

					if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
					{
						upm.OnLeftClickRelease();
					}
				}
			}
		}

		void SceneViewPanel::ImGui_OnResizeRenderImageSize()
		{
			// 그려지는 영역에 맞게 화면 비 재구성
			auto rect = yunutyEngine::graphics::Renderer::SingleInstance().GetResolution();
			float ratio = (float)rect.y / (float)rect.x;
			auto winMin = ImGui::GetWindowContentRegionMin();
			auto winMax = ImGui::GetWindowContentRegionMax();
			ImVec2 newRegion(winMax.x - winMin.x, winMax.y - winMin.y);
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
			cursorPos_InScreenSpace.second = ((renderImageSize.second / 2) - finalPos.y) / renderImageSize.second;
		}

		void SceneViewPanel::ImGui_UpdateWindowSize()
		{
			prevWindowSize.first = currentWindowSize.first;
			prevWindowSize.second = currentWindowSize.second;

			auto size = ImGui::GetContentRegionMax();
			currentWindowSize.first = size.x;
			currentWindowSize.second = size.y;
		}

		bool SceneViewPanel::ImGui_IsCursorInScreen()
		{
			auto curPos = ImGui_GetCursorPosOnPanel();
			ImVec2 startPos = ImGui::GetCursorPos();
			return (curPos.first >= startPos.x && curPos.first <= startPos.x + renderImageSize.first) && (curPos.second >= startPos.y && curPos.second <= startPos.y + renderImageSize.second);
		}

		bool SceneViewPanel::ImGui_IsWindowResize()
		{
			return (prevWindowSize != currentWindowSize);
		}

		std::pair<float, float> SceneViewPanel::ImGui_GetCursorPosOnPanel()
		{
			auto winPos = ImGui::GetWindowPos();
			auto curPos = ImGui::GetMousePos();

			return std::pair<float, float>(curPos.x - winPos.x, curPos.y - winPos.y);
		}

		void SceneViewPanel::Release()
		{
			/// 임시
			static auto& upm = palette::UnitPaletteManager::SingleInstance();
			///

			upm.OnLeftClickRelease();
		}
	}
}
#endif
