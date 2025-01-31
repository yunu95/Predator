/// 2023. 10. 05 김상준
/// 에디터에서 사용할 모든 내용을 모아둔 클래스
/// 해당 레이어를 Release 모드에서 사용하지 않는 것으로
/// Release 모드에서는 에디터에 대한 진입이 불가능 하도록 함

#pragma once

#include <vector>
#include <memory>
#include <functional>
#include <memory>

#include "Layer.h"
#include "CommandManager.h"
#include "EditorCamera.h"
#include "EditorPanel.h"
#include "EditorModule.h"
#include "EditorEvents.h"
#include "MapFileManager.h"

namespace application
{
	namespace editor
	{
		class EditorLayer
			: public Layer
		{
		public:
#ifdef GEN_TESTS
			static void AssignTestInitializer(std::function<void()> testInitializer);
#endif
			static void SetInputControl(bool control);
			static bool GetInputControl();

			virtual void Initialize() override;
			virtual void Update(float ts) override;
			virtual void GUIProgress() override;
			virtual void Finalize() override;
			virtual void OnEvent(EditorEvents& event) override;

			void OnPlayContents();
			void OnPauseContents();
			void OnResumeContents();
			void OnStopContents();

			// Content 레이어의 Initialize 이후에 초기화 되어야 하는
			// GameEngine 과 관련된 내용들을 초기화하는 함수입니다.
			void LateInitialize();

			// Ornament 의 경우, 미리 준비된 FBX 를 사용하는 방식을 사용합니다.
			// 본 함수는 미리 준비된 FBX 로 TemplateData 를 생성하는 함수입니다.
			void ReadyOrnament();

			// Content 레이어에서 Directional Light 가 추가되지 않았을 경우,
			// 내부적으로 DirectionalLight 를 생성하는 함수입니다.
			void CreateDirectionalLight();

		private:
			enum class Panel_List
			{
				HIERARCHY	= 0,
				INSPECTOR,
				PREVIEW,
				MINIMAP,
				SCENEVIEW,
				CAMERAVIEW,
				PALETTE,

				/// Size를 자동으로 넣기 위해 사용하는 enum
				/// 첫 enum 값이 0 이고, 모든 간격이 1일 때에 가능함
				Size
			};

			enum class Module_List
			{
				GlobalConstant = 0,
				TemplateDataEditor,
				ScriptEditor,
				Skill,
				AnimationEditor,
				SoundEditor,

				/// Size를 자동으로 넣기 위해 사용하는 enum
				/// 첫 enum 값이 0 이고, 모든 간격이 1일 때에 가능함
				Size
			};


			static std::function<void()> testInitializer;

			void LoadAllFBXFile();
			void InitSceneGizmo();
			void UI_DrawMenubar();

			CommandManager& cm = CommandManager::GetSingletonInstance();
			EditorCamera& editorCamera = EditorCamera::GetSingletonInstance();
			MapFileManager& mfm = MapFileManager::GetSingletonInstance();

			std::vector<Panel*> editorPanelList;
			std::vector<EditorModule*> editorModuleList;	
		};
	}
}
