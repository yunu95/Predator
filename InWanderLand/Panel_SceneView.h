/// 2023. 10. 04 김상준
/// 에디터에서 편집을 위한 내용을 확인할 수 있는 View 패널

#pragma once

#include "Singleton.h"
#include "EditorPanel.h"
#include "PaletteManager.h"
#include "EditorInputManager.h"

#include <utility>

namespace application
{
	class Application;

	namespace editor
	{
		class EditorCamera;
	}
}

namespace application
{
	namespace editor
	{
		class SceneViewPanel
			: public Panel, public Singleton<SceneViewPanel>
		{
			friend class Singleton<SceneViewPanel>;

		public:
			virtual ~SceneViewPanel();

			virtual void Initialize() override;
			virtual void Update(float ts) override;
			virtual void GUIProgress() override;
			virtual void Finalize() override;
			virtual void OnEvent(EditorEvents& event) override;

			std::pair<unsigned int, unsigned int> GetRenderImageSize() const;
			std::pair<double, double> GetCursorPosInScreenSpace() const;

		private:
			SceneViewPanel();
			
			void ImGui_Update();
			void ImGui_OnResizeRenderImageSize();
			void ImGui_UpdateCursorPosInScreenSpace();
			void ImGui_UpdateWindowSize();
			bool ImGui_IsCursorInScreen();
			bool ImGui_IsWindowResize();
			std::pair<float, float> ImGui_GetCursorPosOnPanel();
			void ImGui_DrawGizmo();
			void Release();

			void ImGui_SceneViewSettings();

			Application* app;
			EditorCamera* ec;
			palette::PaletteManager* pm;
			EditorInputManager& eim = EditorInputManager::GetSingletonInstance();
			std::pair<float, float> prevWindowSize;
			std::pair<float, float> currentWindowSize;
			std::pair<float, float> imageStartPos;
			std::pair<unsigned int, unsigned int> renderImageSize;
			std::pair<double, double> cursorPos_InScreenSpace;

			// flag
		};
	}
}
