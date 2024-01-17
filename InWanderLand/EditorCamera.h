/// 2024. 01. 04 김상준
/// Editor 에서 Scene 내용을 출력 할 카메라
/// 하나의 카메라만 사용할 것이기 때문에 Singleton 으로 구현

#pragma once

#include "Singleton.h"
#include "EditorInputManager.h"

#include "Camera.h"

namespace application
{
	namespace editor
	{
		class EditorCamera
			: public Singleton<EditorCamera>
		{
			friend class Singleton<EditorCamera>;

		public:
			void Initialize(yunutyEngine::graphics::Camera* gameCam);
			void Update(float ts);


			// MainCamera 를 EditorCamera / GameCamera 로 각각 전환합니다.
			void SwitchCam();
			// GameCamera 시점에서 움직이도록 변경
			void SetGamePerspective();
			// 자유 모드로 전환
			void SetFreePerspective();

		private:
			EditorCamera() = default;

			enum class CameraTypeState
			{
				None = 0,
				Editor,
				Game
			};

			enum class CameraPerspectiveState
			{
				None = 0,
				Free,
				Game
			};

			CameraTypeState cameraTState = CameraTypeState::None;
			CameraPerspectiveState cameraPState = CameraPerspectiveState::None;
			yunutyEngine::graphics::Camera* editorCam = nullptr;
			yunutyEngine::graphics::Camera* gameCam = nullptr;
			EditorInputManager& eim = EditorInputManager::GetSingletonInstance();
		};
	}
}

